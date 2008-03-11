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
import sys, os
from IECore import *

from math import pow

class TestCINWriter(unittest.TestCase):
	
	def __verifyImageRGB( self, img ):
	
		self.assertEqual( type(img), ImagePrimitive )	
	
		topLeft =  img.dataWindow.min - img.displayWindow.min
		bottomRight = img.dataWindow.max - img.displayWindow.min
		topRight = V2i( img.dataWindow.max.x, img.dataWindow.min.y) - img.displayWindow.min
		bottomLeft = V2i( img.dataWindow.min.x, img.dataWindow.max.y) - img.displayWindow.min
	
		pixelColorMap = {
			topLeft : V3f( 0, 0, 0 ),
			bottomRight : V3f( 1, 1, 0 ),
			topRight: V3f( 1, 0, 0 ),
			bottomLeft: V3f( 0, 1, 0 ),			
		}
	
		ipe = PrimitiveEvaluator.create( img )
		result = ipe.createResult()	
		
		for pixelColor in pixelColorMap.items() :
		
			found = ipe.pointAtPixel( pixelColor[0], result )
			self.assert_( found )		
			color = V3f(
				result.halfPrimVar( ipe.R() ),
				result.halfPrimVar( ipe.G() ), 
				result.halfPrimVar( ipe.B() )
			)	
								
			self.assert_( ( color - pixelColor[1]).length() < 1.e-3 )
	
	def __makeFloatImage( self, dataWindow, displayWindow, withAlpha = False, dataType = FloatVectorData ) :
	
		img = ImagePrimitive( dataWindow, displayWindow )
		
		w = dataWindow.max.x - dataWindow.min.x + 1
		h = dataWindow.max.y - dataWindow.min.y + 1
		
		area = w * h
		R = dataType( area )
		G = dataType( area )		
		B = dataType( area )
		
		if withAlpha:
			A = dataType( area )
		
		offset = 0
		for y in range( 0, h ) :
			for x in range( 0, w ) :
			
				R[offset] = float(x) / (w - 1)				
				G[offset] = float(y) / (h - 1)
				B[offset] = 0.0
				if withAlpha:
					A[offset] = 0.5
				
				offset = offset + 1				
		
		img["R"] = PrimitiveVariable( PrimitiveVariable.Interpolation.Vertex, R )
		img["G"] = PrimitiveVariable( PrimitiveVariable.Interpolation.Vertex, G )		
		img["B"] = PrimitiveVariable( PrimitiveVariable.Interpolation.Vertex, B )
		
		if withAlpha:
			img["A"] = PrimitiveVariable( PrimitiveVariable.Interpolation.Vertex, A )
		
		return img
		
	def testWrite( self ) :	
		
		displayWindow = Box2i(
			V2i( 0, 0 ),
			V2i( 99, 99 )
		)
		
		dataWindow = displayWindow
		
		for dataType in [ FloatVectorData, HalfVectorData, DoubleVectorData ] :
		
			self.setUp()
		
			img = self.__makeFloatImage( dataWindow, displayWindow, dataType = dataType )
			w = Writer.create( img, "test/IECore/data/cinFiles/output.cin" )
			self.assertEqual( type(w), CINImageWriter )
			w.write()
		
			self.assert_( os.path.exists( "test/IECore/data/cinFiles/output.cin" ) )
			
			# \todo remove this line
			Writer.create( img, "test/IECore/data/cinFiles/output2.cin" ).write()
			
			# Now we've written the image, verify the rgb
			
			img2 = Reader.create( "test/IECore/data/cinFiles/output.cin" ).read()
			self.__verifyImageRGB( img2 )
			
			self.tearDown()	

	def testColorConversion(self):

		r = Reader.create( "test/IECore/data/cinFiles/ramp.cin" )
		img = r.read()
		self.assertEqual(type(img), ImagePrimitive)
		w = Writer.create(img, "test/IECore/data/cinFiles/output.cin")
		self.assertEqual(type(w), CINImageWriter)
		w.write()
		w = None
		r = Reader.create( "test/IECore/data/cinFiles/output.cin" )
		img2 = r.read()
		self.assertEqual(type(img2), ImagePrimitive)
		self.assertEqual( img, img2 )

	def tearDown( self ) :
	
		if os.path.isfile( "test/IECore/data/cinFiles/output.cin") :
			os.remove( "test/IECore/data/cinFiles/output.cin" )
					
       			
if __name__ == "__main__":
	unittest.main()   
	
