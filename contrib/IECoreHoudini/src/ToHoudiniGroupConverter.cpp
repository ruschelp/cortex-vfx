//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010-2011, Image Engine Design Inc. All rights reserved.
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

#include "IECore/CompoundParameter.h"
#include "IECore/NullObject.h"
#include "IECore/SimpleTypedData.h"
#include "IECore/TransformOp.h"

#include "ToHoudiniGroupConverter.h"

using namespace IECore;
using namespace IECoreHoudini;

IE_CORE_DEFINERUNTIMETYPED( ToHoudiniGroupConverter );

ToHoudiniGeometryConverter::Description<ToHoudiniGroupConverter> ToHoudiniGroupConverter::m_description( GroupTypeId );

ToHoudiniGroupConverter::ToHoudiniGroupConverter( const IECore::VisibleRenderable *renderable ) :
	ToHoudiniGeometryConverter( renderable, "Converts an IECore::Group to a Houdini GU_Detail." )
{
	m_transformParameter = new M44fParameter(
		"transform",
		"The matrix used to transform the children.",
		new M44fData()
	);
	
	parameters()->addParameter( m_transformParameter );
}

ToHoudiniGroupConverter::~ToHoudiniGroupConverter()
{
}

M44fParameter *ToHoudiniGroupConverter::transformParameter()
{
	return m_transformParameter;
}

const M44fParameter *ToHoudiniGroupConverter::transformParameter() const
{
	return m_transformParameter;
}

bool ToHoudiniGroupConverter::doConversion( const VisibleRenderable *renderable, GU_Detail *geo ) const
{
	const Group *group = IECore::runTimeCast<const Group>( renderable );
	if ( !group )
	{
		return false;
	}
	
	Imath::M44f transform = ( runTimeCast<const M44fData>( m_transformParameter->getValue() ) )->readable();
	const Transform *groupTransform = group->getTransform();
	if ( groupTransform )
	{
		transform = transform * groupTransform->transform();
	}
	
	TransformOpPtr transformOp = new TransformOp();
	M44fDataPtr transformData = new M44fData( transform );
	transformOp->matrixParameter()->setValue( transformData );
	
	StringDataPtr groupName = group->blindData()->member<StringData>( "name" );
	
	const Group::ChildContainer &children = group->children();
	for ( Group::ChildContainer::const_iterator it=children.begin(); it != children.end(); it++ )
	{
		ConstVisibleRenderablePtr child = *it;
		
		ConstPrimitivePtr primitive = runTimeCast<const Primitive>( child );
		if ( primitive )
		{
			transformOp->inputParameter()->setValue( constPointerCast<Primitive>( primitive ) );
			child = staticPointerCast<VisibleRenderable>( transformOp->operate() );
		}
		
		ToHoudiniGeometryConverterPtr converter = ToHoudiniGeometryConverter::create( child );
		if ( !converter )
		{
			continue;
		}
		
		ToHoudiniGroupConverter *groupConverter = runTimeCast<ToHoudiniGroupConverter>( converter );
		if ( groupConverter )
		{
			groupConverter->transformParameter()->setValue( transformData );
		}
		
		size_t origNumPrims = geo->getNumPrimitives();
		
		GU_DetailHandle handle;
		handle.allocateAndSet( geo, false );
		
		if ( !converter->convert( handle ) )
		{
			continue;
		}
		
		StringDataPtr childName = child->blindData()->member<StringData>( "name" );
		if ( !childName && !groupName )
		{
			continue;
		}
		
		std::string name = childName ? childName->readable() : groupName->readable();
		GA_ElementGroup *childGroup = geo->findPrimitiveGroup( name.c_str() );
		if ( !childGroup || childGroup->classType() != GA_GROUP_PRIMITIVE )
		{
			childGroup = geo->createElementGroup( GA_ATTRIB_PRIMITIVE, name.c_str() );
		}
		
		childGroup->addRange( geo->getPrimitiveRangeSlice( origNumPrims ) );
	}
	
	return true;
}
