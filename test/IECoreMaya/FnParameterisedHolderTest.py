##########################################################################
#
#  Copyright (c) 2008-2009, Image Engine Design Inc. All rights reserved.
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

from __future__ import with_statement

import IECore
import IECoreMaya
import MayaUnitTest
import unittest
import maya.cmds
import maya.OpenMaya

class TestOp( IECore.Op ) :

	def __init__( self ) :
	
		IECore.Op.__init__( self, "TestOp", "Tests stuff",
			IECore.IntParameter(
				name = "result",
				description = "",
				defaultValue = 0
			)
		)
		
		self.parameters().addParameters(
			[
				IECore.IntParameter(
					name = "i",
					description = "i",
					defaultValue = 1
				),
			]
		)

class FnParameterisedHolderTest( unittest.TestCase ) :

	def test( self ) :
	
		node = maya.cmds.createNode( "ieOpHolderNode" )
		fnPH = IECoreMaya.FnParameterisedHolder( node )
		self.assertEqual( fnPH.getParameterised(), ( None, "", 0, "" ) )

		op = TestOp()
		fnPH.setParameterised( op )
		self.assertEqual( fnPH.getParameterised(), ( op, "", 0, "" ) )
		
		iPlug = fnPH.parameterPlug( op["i"] )
		self.assert_( isinstance( iPlug, maya.OpenMaya.MPlug ) )
		self.assert_( iPlug.asInt(), 1 )
		
		self.assert_( fnPH.plugParameter( iPlug ).isSame( op["i"] ) )
		
		iPlug.setInt( 2 )
		fnPH.setParameterisedValue( op["i"] )
		self.assert_( op["i"].getNumericValue(), 2 )
		
		op["i"].setNumericValue( 3 )
		fnPH.setNodeValue( op["i"] )
		self.assert_( iPlug.asInt(), 3 )
		
		iPlug.setInt( 10 )
		fnPH.setParameterisedValues()
		self.assert_( op["i"].getNumericValue(), 10 )
		
		op["i"].setNumericValue( 11 )
		fnPH.setNodeValues()
		self.assert_( iPlug.asInt(), 11 )

	def testFullPathName( self ) :

		node = maya.cmds.createNode( "ieOpHolderNode" )
		fnPH = IECoreMaya.FnParameterisedHolder( node )
		self.assertEqual( node, fnPH.fullPathName() )
		
		procedural = maya.cmds.createNode( "ieProceduralHolder" )
		fnPH = IECoreMaya.FnParameterisedHolder( procedural )
		self.assertEqual( maya.cmds.ls( procedural, long=True )[0], fnPH.fullPathName() )
		
	def testPlugParameterWithNonUniqueNames( self ) :
	
		node = maya.cmds.createNode( "ieProceduralHolder" )
		node2 = maya.cmds.createNode( "ieProceduralHolder" )
		
		node = maya.cmds.ls( maya.cmds.rename( node, "iAmNotUnique" ), long=True )[0]
		node2 = maya.cmds.ls( maya.cmds.rename( node2, "iAmNotUnique" ), long=True )[0]
				
		fnPH = IECoreMaya.FnProceduralHolder( node )
		proc = IECore.ReadProcedural()
		fnPH.setParameterised( proc )
		self.assert_( fnPH.getParameterised()[0].isSame( proc ) )
		
		fnPH2 = IECoreMaya.FnProceduralHolder( node2 )
		proc2 = IECore.ReadProcedural()
		fnPH2.setParameterised( proc2 )
		self.assert_( fnPH2.getParameterised()[0].isSame( proc2 ) )
		
		# check that each function set references a different node.
		self.assert_( fnPH.object()!=fnPH2.object() )
		self.assert_( fnPH.fullPathName()!=fnPH2.fullPathName() )
				
		plug = fnPH.parameterPlug( proc["motion"]["blur"] )
		plug2 = fnPH2.parameterPlug( proc2["motion"]["blur"] )
				
		self.assertEqual( plug.node(), fnPH.object() )
		self.assertEqual( plug2.node(), fnPH2.object() )
		
		self.assertEqual( fnPH.parameterPlugPath( proc["motion"]["blur"] ), "|transform1|iAmNotUnique.parm_motion_blur" )
		self.assertEqual( fnPH2.parameterPlugPath( proc2["motion"]["blur"] ), "|transform2|iAmNotUnique.parm_motion_blur" )
		
		self.assert_( maya.cmds.isConnected( "time1.outTime", fnPH.parameterPlugPath( proc["files"]["frame"] ), iuc=True ) )
		self.assert_( maya.cmds.isConnected( "time1.outTime", fnPH2.parameterPlugPath( proc2["files"]["frame"] ), iuc=True ) )
	
	def testSetNodeValueUndo( self ) :
	
		node = maya.cmds.createNode( "ieOpHolderNode" )
		fnPH = IECoreMaya.FnParameterisedHolder( node )
		op = TestOp()
		fnPH.setParameterised( op )
		iPlug = fnPH.parameterPlug( op["i"] )
		
		self.assertEqual( op["i"].getNumericValue(), 1 )
		self.assertEqual( iPlug.asInt(), 1 )
		
		self.assert_( maya.cmds.undoInfo( query=True, state=True ) )
		
		op["i"].setNumericValue( 10 )
		fnPH.setNodeValues()
		self.assertEqual( op["i"].getNumericValue(), 10 )
		
		maya.cmds.undo()
		self.assertEqual( op["i"].getNumericValue(), 1 )
		
				
if __name__ == "__main__":
	MayaUnitTest.TestProgram()
