//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007-2009, Image Engine Design Inc. All rights reserved.
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

#include "boost/python.hpp"

#include "IECore/ValidatedStringParameter.h"
#include "IECore/CompoundObject.h"
#include "IECore/bindings/ParameterBinding.h"
#include "IECore/bindings/IntrusivePtrPatch.h"

using namespace std;
using namespace boost;
using namespace boost::python;

namespace IECore
{

static ValidatedStringParameterPtr validatedStringParameterConstructor( const std::string &name, const std::string &description,
	const std::string &regex, const std::string &regexDescription,
	const std::string &defaultValue, bool allowEmptyString, const object &presets, bool presetsOnly, object userData )
{
 	ValidatedStringParameter::PresetsContainer p = parameterPresets<ValidatedStringParameter::PresetsContainer>( presets );
	// get the optional userData parameter.
	ConstCompoundObjectPtr ptrUserData = 0;
	if (userData != object()) {
		extract<CompoundObjectPtr> elem(userData);
		// try if elem is an exact CompoundObjectPtr
		if (elem.check()) {
			ptrUserData = elem();
		} else {
			// now try for ConstCompoundObjectPtr
			extract<ConstCompoundObjectPtr> elem(userData);
			if (elem.check()) {
				ptrUserData = elem();
			} else {
			   	PyErr_SetString(PyExc_TypeError, "Parameter userData is not an instance of CompoundObject!");
			  	throw_error_already_set();
				ValidatedStringParameterPtr res;
				return res;
			}
		}
	}
	return new ValidatedStringParameter( name, description, regex, regexDescription, defaultValue, allowEmptyString, p, presetsOnly, ptrUserData );
}

void bindValidatedStringParameter()
{

	typedef class_< ValidatedStringParameter, ValidatedStringParameterPtr, boost::noncopyable, bases<StringParameter> > ValidatedStringParameterPyClass;
	ValidatedStringParameterPyClass( "ValidatedStringParameter", no_init )
		.def( "__init__", make_constructor( &validatedStringParameterConstructor, default_call_policies(), ( boost::python::arg_( "name" ), boost::python::arg_( "description" ), boost::python::arg_( "regex" ), boost::python::arg_( "regexDescription" ), boost::python::arg_( "defaultValue" ) = string( "" ), boost::python::arg_( "allowEmptyString" ) = true, boost::python::arg_( "presets" ) = boost::python::tuple(), boost::python::arg_( "presetsOnly" ) = false, boost::python::arg_( "userData" ) = object() ) ) )
		.add_property( "regex", make_function( &ValidatedStringParameter::regex, return_value_policy<copy_const_reference>() ) )
		.add_property( "regexDescription", make_function( &ValidatedStringParameter::regexDescription, return_value_policy<copy_const_reference>() ) )
		.add_property( "allowEmptyString", &ValidatedStringParameter::allowEmptyString )
		.IE_COREPYTHON_DEFPARAMETERWRAPPERFNS( ValidatedStringParameter )
		.IE_COREPYTHON_DEFRUNTIMETYPEDSTATICMETHODS( ValidatedStringParameter )
	;
	INTRUSIVE_PTR_PATCH( ValidatedStringParameter, ValidatedStringParameterPyClass );
	implicitly_convertible<ValidatedStringParameterPtr, StringParameterPtr>();

}

} // namespace IECore
