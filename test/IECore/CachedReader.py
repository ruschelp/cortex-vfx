##########################################################################
#
#  Copyright (c) 2007-2011, Image Engine Design Inc. All rights reserved.
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
import os

class CachedReaderTest( unittest.TestCase ) :

	def test( self ) :

		r = CachedReader( SearchPath( "./", ":" ), 100 * 1024 * 1024 )

		o = r.read( "test/IECore/data/cobFiles/compoundData.cob" )
		self.assertEqual( o.typeName(), "CompoundData" )
		self.assertEqual( r.memoryUsage(), o.memoryUsage() )
		self.failUnless( r.cached( "test/IECore/data/cobFiles/compoundData.cob" ) )

		oo = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( r.memoryUsage(), o.memoryUsage() + oo.memoryUsage() )
		self.failUnless( r.cached( "test/IECore/data/cobFiles/compoundData.cob" ) )
		self.failUnless( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		oo = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( r.memoryUsage(), o.memoryUsage() + oo.memoryUsage() )
		self.failUnless( r.cached( "test/IECore/data/cobFiles/compoundData.cob" ) )
		self.failUnless( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		self.assertRaises( RuntimeError, r.read, "doesNotExist" )
		self.assertRaises( RuntimeError, r.read, "doesNotExist" )

		# Here, the cache should throw away "o" (because there isn't enough room for it, and it was the least
		# recently used) leaving us with just "oo"
		r.maxMemory = oo.memoryUsage() + ( o.memoryUsage() / 2 )
		self.assertEqual( r.memoryUsage(), oo.memoryUsage() )
		self.failIf( r.cached( "test/IECore/data/cobFiles/compoundData.cob" ) )
		self.failUnless( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		# Here, the cache should throw away "oo" (because there isn't enough room for it, and it was the least
		# recently used) leaving us empty
		r.maxMemory = oo.memoryUsage() / 2
		self.assertEqual( r.memoryUsage(), 0 )
		self.failIf( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		r.maxMemory = oo.memoryUsage() * 2
		oo = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		r.clear()
		self.assertEqual( r.memoryUsage(), 0 )
		self.failIf( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		oo = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( r.memoryUsage(), oo.memoryUsage() )
		self.failUnless( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )
		r.clear( "I don't exist" )
		self.failUnless( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )
		self.assertEqual( r.memoryUsage(), oo.memoryUsage() )
		r.clear( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( r.memoryUsage(), 0 )
		self.failIf( r.cached( "test/IECore/data/pdcFiles/particleShape1.250.pdc" ) )

		# testing insert.
		r.insert( "test/IECore/data/pdcFiles/particleShape1.250.pdc", oo )
		self.assertEqual( r.memoryUsage(), oo.memoryUsage() )
		o2 = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( oo, o2 )
		# testing overwritting existing item with insert
		r.insert( "test/IECore/data/pdcFiles/particleShape1.250.pdc", o )
		self.assertEqual( r.memoryUsage(), o.memoryUsage() )
		o2 = r.read( "test/IECore/data/pdcFiles/particleShape1.250.pdc" )
		self.assertEqual( o, o2 )
		
		# test clear after failed load
		self.assertRaises( RuntimeError, r.read, "/i/dont/exist" )
		r.clear( "/i/dont/exist" )

	def testRepeatedFailures( self ) :
	
		def check( fileName ) :
		
			r = CachedReader( SearchPath( "./", ":" ), 100 * 1024 * 1024 )
			firstException = None
			try :
				r.read( fileName )		
			except Exception, e :
				firstException = str( e )

			self.assert_( firstException )

			secondException = None
			try :
				r.read( fileName )		
			except Exception, e :
				secondException = str( e )

			self.assert_( secondException )

			# we want the second exception to be different, as the CachedReader
			# shouldn't be wasting time attempting to load files again when
			# it failed the first time
			self.assertNotEqual( firstException, secondException )
			
		check( "iDontExist" )
		check( "include/IECore/IECore.h" )

	def testChangeSearchPaths( self ) :
	
		# read a file from one path
		r = CachedReader( SearchPath( "./test/IECore/data/cachedReaderPath1", ":" ), 100 * 1024 * 1024 )
		
		o1 = r.read( "file.cob" )
		
		self.assertEqual( o1.value, 1 )
		self.failUnless( r.cached( "file.cob" ) )
		
		# read a file of the same name from a different path
		r.searchPath = SearchPath( "./test/IECore/data/cachedReaderPath2", ":" )
		self.failIf( r.cached( "file.cob" ) )
		
		o2 = r.read( "file.cob" )
		
		self.assertEqual( o2.value, 2 )
		self.failUnless( r.cached( "file.cob" ) )
		
		# set the paths to the same as before and check we didn't obliterate the cache unecessarily
		r.searchPath = SearchPath( "./test/IECore/data/cachedReaderPath2", ":" )
		self.failUnless( r.cached( "file.cob" ) )		

	def testDefault( self ) :

		os.environ["IECORE_CACHEDREADER_PATHS"] = "a:test:path"
		os.environ["IECORE_CACHEDREADER_MEMORY"] = "200"

		r = CachedReader.defaultCachedReader()
		r2 = CachedReader.defaultCachedReader()
		self.assert_( r.isSame( r2 ) )
		self.assertEqual( r.maxMemory, 1024 * 1024 * 200 )
		self.assertEqual( r.searchPath, SearchPath( "a:test:path", ":" ) )
		
	def testPostProcessing( self ) :
	
		r = CachedReader( SearchPath( "./test/IECore/data/cobFiles", ":" ), 100 * 1024 * 1024 )
		m = r.read( "polySphereQuads.cob" )
		self.failUnless( 4 in m.verticesPerFace )

		r = CachedReader( SearchPath( "./test/IECore/data/cobFiles", ":" ), 100 * 1024 * 1024, TriangulateOp() )
		m = r.read( "polySphereQuads.cob" )
		for v in m.verticesPerFace :
			self.assertEqual( v, 3 )
			
	def testPostProcessingFailureMode( self ) :
	
		class PostProcessor( ModifyOp ) :
		
			def __init__( self ) :
			
				ModifyOp.__init__( self, "", Parameter( "result", "", NullObject() ), Parameter( "input", "", NullObject() ) )
				
			def modify( self, obj, args ) :
			
				raise Exception( "I am a very naughty op" )
				
		r = CachedReader( SearchPath( "./test/IECore/data/cobFiles", ":" ), 100 * 1024 * 1024, PostProcessor() )
		
		firstException = None
		try :
			m = r.read( "polySphereQuads.cob" )
		except Exception, e :
			firstException = str( e )
		
		self.failUnless( firstException is not None )
				
		secondException = None
		try :
			m = r.read( "polySphereQuads.cob" )
		except Exception, e :
			secondException = str( e )
		
		self.failUnless( secondException is not None )
		
		# we want the second exception to be different, as the CachedReader
		# shouldn't be wasting time attempting to load files again when
		# it failed the first time
		self.assertNotEqual( firstException, secondException )
		
if __name__ == "__main__":
    unittest.main()
