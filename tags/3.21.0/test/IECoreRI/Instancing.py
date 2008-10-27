##########################################################################
#
#  Copyright (c) 2007, Image Engine Design Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of Image Engine Design nor the names of any
#       other contributors to this software may be used to endorse or
#       promote products derived from this software without specific prior
#       written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import unittest
from IECore import *
import IECoreRI
import os

class InstancingTest( unittest.TestCase ) :

	def test( self ) :
	
		r = IECoreRI.Renderer( "test/IECoreRI/output/instancing.rib" )		
		
		r.command( "ri:objectBegin", { "name" : StringData( "myObject" ) } )
		r.geometry( "teapot", {}, {} )
		r.command( "ri:objectEnd", {} )
		
		r.worldBegin()
		
		r.command( "ri:objectInstance", { "name" : StringData( "myObject" ) } )
		r.worldEnd()
	
		rib = "".join( open( "test/IECoreRI/output/instancing.rib" ).readlines() )
		
		self.assert_( "ObjectBegin" in rib )
		self.assert_( "ObjectEnd" in rib )
		self.assert_( "ObjectInstance" in rib )
		
	
	def test2( self ) :
	
		r = IECoreRI.Renderer( "test/IECoreRI/output/instancing2.rib" )		
		
		r.command( "objectBegin", { "name" : StringData( "myObject" ) } )
		r.geometry( "teapot", {}, {} )
		r.command( "objectEnd", {} )
		
		r.worldBegin()
		
		r.command( "objectInstance", { "name" : StringData( "myObject" ) } )
		
		r.worldEnd()

		rib = "".join( open( "test/IECoreRI/output/instancing2.rib" ).readlines() )
		
		self.assert_( "ObjectBegin" in rib )
		self.assert_( "ObjectEnd" in rib )
		self.assert_( "ObjectInstance" in rib )

	def testInstancingAPI( self ) :
	
		r = IECoreRI.Renderer( "test/IECoreRI/output/instancing3.rib" )		
		
		r.instanceBegin( "myObject", {} )
		r.geometry( "teapot", {}, {} )
		r.instanceEnd()
		
		r.worldBegin()
		
		r.instance( "myObject" )
		
		r.worldEnd()

		rib = "".join( open( "test/IECoreRI/output/instancing3.rib" ).readlines() )
		
		self.assert_( "ObjectBegin" in rib )
		self.assert_( "ObjectEnd" in rib )
		self.assert_( "ObjectInstance" in rib )
				
	def tearDown( self ) :
	
		files = [
			"test/IECoreRI/output/instancing.rib",
			"test/IECoreRI/output/instancing2.rib",
			"test/IECoreRI/output/instancing3.rib",
		]
		
		for f in files :
			if os.path.exists( f ):
				os.remove( f )
					
if __name__ == "__main__":
    unittest.main()   
