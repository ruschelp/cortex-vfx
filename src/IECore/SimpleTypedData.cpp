//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007-2012, Image Engine Design Inc. All rights reserved.
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

#include "IECore/SimpleTypedData.h"
#include "IECore/TypedData.inl"

#include <iostream>

namespace IECore
{

LongDataAlias::TypeDescription<IntData> LongDataAlias::m_typeDescription( LongDataTypeId, "LongData" );

#define IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( TNAME, TID, N )	\
	IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( TNAME, TID ) \
	\
	template<> \
	void TNAME::save( SaveContext *context ) const \
	{ \
		Data::save( context ); \
		assert( baseSize() == N ); \
		IndexedIOInterfacePtr container = context->rawContainer(); \
		container->write( "value", TNAME::baseReadable(), TNAME::baseSize() ); \
	} \
	\
	template<> \
	void TNAME::load( LoadContextPtr context ) \
	{ \
		Data::load( context ); \
		assert( ( sizeof( TNAME::ValueType ) / sizeof( TNAME::BaseType ) ) == N ); \
		IndexedIOInterfacePtr container; \
		TNAME::BaseType *p = TNAME::baseWritable(); \
		try \
		{ \
			container = context->rawContainer(); \
			container->read( "value", p, N ); \
		} \
		catch( ... ) \
		{ \
			unsigned int v = 0;	\
			container = context->container( staticTypeName(), v ); \
			container->read( "value", p, N ); \
		} \
	}

#define IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( TNAME ) \
	template<> \
	TNAME::TypedData() \
		:	m_data( ValueType( 0 ) ) \
	{ \
	}
	
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( BoolData, BoolDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( FloatData, FloatDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( DoubleData, DoubleDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( IntData, IntDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( UIntData, UIntDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( CharData, CharDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( UCharData, UCharDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( HalfData, HalfDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( ShortData, ShortDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( UShortData, UShortDataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( Int64Data, Int64DataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( UInt64Data, UInt64DataTypeId )
IECORE_RUNTIMETYPED_DEFINETEMPLATESPECIALISATION( StringData, StringDataTypeId )

IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V2iData, V2iDataTypeId, 2 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V3iData, V3iDataTypeId, 3 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V2fData, V2fDataTypeId, 2 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V3fData, V3fDataTypeId, 3 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V2dData, V2dDataTypeId, 2 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( V3dData, V3dDataTypeId, 3 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Color3fData, Color3fDataTypeId, 3 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Color4fData, Color4fDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Color3dData, Color3dDataTypeId, 3 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Color4dData, Color4dDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box2iData, Box2iDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box3iData, Box3iDataTypeId, 6 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box2fData, Box2fDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box3fData, Box3fDataTypeId, 6 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box2dData, Box2dDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( Box3dData, Box3dDataTypeId, 6 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( M33fData, M33fDataTypeId, 9 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( M33dData, M33dDataTypeId, 9 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( M44fData, M44fDataTypeId, 16 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( M44dData, M44dDataTypeId, 16 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( QuatfData, QuatfDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( QuatdData, QuatdDataTypeId, 4 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( LineSegment3fData, LineSegment3fDataTypeId, 6 )
IE_CORE_DEFINEIMATHTYPEDDATASPECIALISATION( LineSegment3dData, LineSegment3dDataTypeId, 6 )

IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V2iData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V3iData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V2fData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V3fData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V2dData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( V3dData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( Color3fData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( Color4fData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( Color3dData )
IECORE_DEFINE_ZERO_INITIALISED_CONSTRUCTOR( Color4dData )

template<>
void StringData::memoryUsage( Object::MemoryAccumulator &accumulator ) const
{
	Data::memoryUsage( accumulator );
	accumulator.accumulate( &readable(), readable().capacity() );
}

template<>
void TypedData<bool>::save( SaveContext *context ) const
{
	Data::save( context );
	IndexedIOInterfacePtr container = context->rawContainer();
	unsigned char c = readable();
	container->write( "value", c );
}

template<>
void TypedData<bool>::load( LoadContextPtr context )
{
	Data::load( context );
	unsigned char c;
	try
	{
		// optimised format for new files
		IndexedIOInterfacePtr container = context->rawContainer();
		container->read( "value", c );
	}
	catch( ... )
	{
		// backwards compatibility with old files
		unsigned int v = 0;
		IndexedIOInterfacePtr container = context->container( staticTypeName(), v );
		container->read( "value", c );
	}

	writable() = c;
}

template<>
void TypedData<short>::save( SaveContext *context ) const
{
	Data::save( context );
	IndexedIOInterfacePtr container = context->rawContainer();
	int c = readable();
	container->write( "value", c );
}

template<>
void TypedData<short>::load( LoadContextPtr context )
{
	Data::load( context );
	int c;
	try
	{
		// optimised format for new files
		IndexedIOInterfacePtr container = context->rawContainer();
		container->read( "value", c );
	}
	catch( ... )
	{
		// backwards compatibility with old files
		unsigned int v = 0;
		IndexedIOInterfacePtr container = context->container( staticTypeName(), v );
		container->read( "value", c );
	}

	writable() = static_cast<short>( c );
}

template<>
void TypedData<unsigned short>::save( SaveContext *context ) const
{
	Data::save( context );
	IndexedIOInterfacePtr container = context->rawContainer();
	unsigned int c = readable();
	container->write( "value", c );
}

template<>
void TypedData<unsigned short>::load( LoadContextPtr context )
{
	Data::load( context );
	unsigned int c;
	try
	{
		// optimised format for new files
		IndexedIOInterfacePtr container = context->rawContainer();
		container->read( "value", c );
	}
	catch( ... )
	{
		// backwards compatibility with old files
		unsigned int v = 0;
		IndexedIOInterfacePtr container = context->container( staticTypeName(), v );
		container->read( "value", c );
	}

	writable() = static_cast<unsigned short>( c );
}

template<>
TypedData<LineSegment3f>::TypedData()
	:	m_data( LineSegment3f( Imath::V3f( 0 ), Imath::V3f( 1, 0, 0 ) ) )
{
}

template<>
TypedData<LineSegment3d>::TypedData()
	:	m_data( LineSegment3d( Imath::V3d( 0 ), Imath::V3d( 1, 0, 0 ) ) )
{
}

template<>
void SimpleDataHolder<LineSegment3f>::hash( MurmurHash &h ) const
{
	h.append( readable().p0 );
	h.append( readable().p1 );
}

template<>
void SimpleDataHolder<LineSegment3d>::hash( MurmurHash &h ) const
{
	h.append( readable().p0 );
	h.append( readable().p1 );
}

template class TypedData<bool>;
template class TypedData<float>;
template class TypedData<double>;
template class TypedData<int>;
template class TypedData<unsigned int>;
template class TypedData<char>;
template class TypedData<unsigned char>;
template class TypedData<short>;
template class TypedData<unsigned short>;
template class TypedData<int64_t>;
template class TypedData<uint64_t>;
template class TypedData<std::string>;
template class TypedData<half>;
template class TypedData<Imath::V2i>;
template class TypedData<Imath::V3i>;
template class TypedData<Imath::V2f>;
template class TypedData<Imath::V3f>;
template class TypedData<Imath::V2d>;
template class TypedData<Imath::V3d>;
template class TypedData<Imath::Color3f>;
template class TypedData<Imath::Color4f>;
template class TypedData<Imath::Color3<double> >;
template class TypedData<Imath::Color4<double> >;
template class TypedData<Imath::Box2i>;
template class TypedData<Imath::Box3i>;
template class TypedData<Imath::Box2f>;
template class TypedData<Imath::Box3f>;
template class TypedData<Imath::Box2d>;
template class TypedData<Imath::Box3d>;
template class TypedData<Imath::M33f>;
template class TypedData<Imath::M33d>;
template class TypedData<Imath::M44f>;
template class TypedData<Imath::M44d>;
template class TypedData<Imath::Quatf>;
template class TypedData<Imath::Quatd>;
template class TypedData<LineSegment3f>;
template class TypedData<LineSegment3d>;

} // namespace IECore
