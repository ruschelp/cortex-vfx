//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Image Engine Design nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include "IECoreMaya/FromMayaGroupConverter.h"
#include "IECoreMaya/FromMayaMeshConverter.h"
#include "IECoreMaya/FromMayaRenderableConverterUtil.h"
#include "IECoreMaya/MatrixTraits.h"

#include "IECore/Group.h"
#include "IECore/MatrixOps.h"
#include "IECore/MatrixTransform.h"
#include "IECore/CompoundObject.h"

#include "maya/MFn.h"
#include "maya/MFnDagNode.h"

using namespace IECoreMaya;
using namespace IECore;
using namespace std;
using namespace Imath;

FromMayaObjectConverter::FromMayaObjectConverterDescription<FromMayaGroupConverter> FromMayaGroupConverter::m_description( MFn::kTransform, GroupTypeId );

FromMayaGroupConverter::FromMayaGroupConverter( const MObject &object )
	:	FromMayaObjectConverter( "FromMayaGroupConverter", "Converts transforms to Group objects.", object )
{
	
	FromMayaRenderableConverterUtilPtr mu = new FromMayaRenderableConverterUtil();
	parameters()->addParameters( mu->parameters()->orderedParameters().begin(), mu->parameters()->orderedParameters().end() );

	BoolParameterPtr ignoreIntermediateObjects = new BoolParameter(
		"ignoreIntermediateObjects",
		"If true, all the intermediate objects will be excluded from the conversion.",
		true
	);
	parameters()->addParameter( ignoreIntermediateObjects );

	FromMayaObjectConverterPtr m = new FromMayaMeshConverter( MObject() );
	m_meshParameters = new CompoundParameter( "meshes", "Parameters to control the conversion of meshes.", m->parameters()->orderedParameters().begin(), m->parameters()->orderedParameters().end() );
	parameters()->addParameter( m_meshParameters );

}
		
IECore::ObjectPtr FromMayaGroupConverter::doConversion( const MObject &object, IECore::ConstCompoundObjectPtr operands ) const
{
	bool ignoreIntermediate = false;
	CompoundObject::ObjectMap::const_iterator it = operands->members().find( "ignoreIntermediateObjects" );
	if ( it != operands->members().end() )
	{
		ignoreIntermediate = boost::static_pointer_cast< BoolData >( it->second )->readable();
	}

	if( object.apiType()==MFn::kTransform )
	{
		MFnDagNode fnDagNode( object );
		
	 	GroupPtr result = new Group();		
		result->setTransform( new MatrixTransform( matConvert<MMatrix, M44f>( fnDagNode.transformationMatrix() ) ) );
		
		unsigned int n = fnDagNode.childCount();
		for( unsigned int i=0; i<n; i++ )
		{
			MObject child = fnDagNode.child( i );

			if ( ignoreIntermediate )
			{
				// ignore intermediate objects
				MFnDagNode fnChildDagNode( child );
				if ( fnChildDagNode.isIntermediateObject() )
				{
					continue;
				}
			}

			VisibleRenderablePtr v = runTimeCast<VisibleRenderable>( doConversion( child, operands ) );
			if( v )
			{
				result->addChild( v );
			}
		}
		FromMayaRenderableConverterUtil::addBlindDataAttributes( operands, object, result );
		return result;
	}
	else
	{
		FromMayaObjectConverterPtr c = 0;
		if( object.apiType()==MFn::kMesh )
		{
			c = new FromMayaMeshConverter( object );
			c->parameters()->setValue( m_meshParameters->getValue() );
		}
		else
		{
			c = FromMayaObjectConverter::create( object, VisibleRenderable::staticTypeId() );
		}
		
		if( c )
		{
			return c->convert();
		}
	}

	return 0;
}
