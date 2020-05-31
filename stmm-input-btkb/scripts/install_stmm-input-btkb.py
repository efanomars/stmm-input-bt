#!/usr/bin/env python3

#  Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, see <http://www.gnu.org/licenses/>

# File:   install_stmm-input-btkb.py

# Compiles and installs the stmm-input-btkb application.

import sys
import os
import subprocess

def main():
	import argparse
	oParser = argparse.ArgumentParser()
	oParser.add_argument("-b", "--buildtype", help="build type (default=Release)"\
						, choices=['Debug', 'Release', 'MinSizeRel', 'RelWithDebInfo']\
						, default="Release", dest="sBuildType")
	oParser.add_argument("--installdir", help="install dir (default=/usr/local)", metavar='INSTALLDIR'\
						, default="/usr/local", dest="sInstallDir")
	oParser.add_argument("--schemasdir", help="gsettings schemas prefix (default=/usr)", metavar='SCHEMASDIR'\
						, default="/usr", dest="sSchemasDir")
	oParser.add_argument("--no-configure", help="don't configure", action="store_true"\
						, default=False, dest="bDontConfigure")
	oParser.add_argument("--no-make", help="don't make", action="store_true"\
						, default=False, dest="bDontMake")
	oParser.add_argument("--no-install", help="don't install", action="store_true"\
						, default=False, dest="bDontInstall")
	oParser.add_argument("--no-compile-schemas", help="don't compile schemas", action="store_true"\
						, default=False, dest="bDontCompileSchemas")
	oParser.add_argument("--no-sudo", help="don't use sudo to install", action="store_true"\
						, default=False, dest="bDontSudo")
	oParser.add_argument("--no-icons", help="don't install icons (implies --no-launcher)", action="store_true"\
						, default=False, dest="bNoIcons")
	oParser.add_argument("--no-launcher", help="don't install launcher", action="store_true"\
						, default=False, dest="bNoLauncher")
	oParser.add_argument("--no-man", help="don't install man page", action="store_true"\
						, default=False, dest="bNoMan")
	oArgs = oParser.parse_args()

	sInstallDir = os.path.abspath(os.path.expanduser(oArgs.sInstallDir))
	sSchemasDir = os.path.abspath(os.path.expanduser(oArgs.sSchemasDir))

	sScriptDir = os.path.dirname(os.path.abspath(__file__))
	#print("sScriptDir:" + sScriptDir)
	os.chdir(sScriptDir)
	os.chdir("..")
	#
	sInstallDir = "-D CMAKE_INSTALL_PREFIX=" + sInstallDir
	#print("sInstallDir:" + sInstallDir)
	#
	sCMakeSchemasDir = "-D STMM_GSETTINGS_SCHEMA_INSTALL_PREFIX=" + sSchemasDir
	#print("sSchemasDir:" + sSchemasDir)
	#
	sBuildType = "-D CMAKE_BUILD_TYPE=" + oArgs.sBuildType
	#print("sBuildType:" + sBuildType)

	if oArgs.bDontSudo:
		sSudo = ""
	else:
		sSudo = "sudo"

	sInstallMan = "-D STMM_INSTALL_MAN_PAGE="
	if (oArgs.bNoMan):
		sInstallMan += "OFF"
	else:
		sInstallMan += "ON"

	sInstallIcons = "-D STMM_INSTALL_ICONS="
	if (oArgs.bNoIcons):
		sInstallIcons += "OFF"
		oArgs.bNoLauncher = True
	else:
		sInstallIcons += "ON"

	sInstallLauncher = "-D STMM_INSTALL_LAUNCHER="
	if (oArgs.bNoLauncher):
		sInstallLauncher += "OFF"
	else:
		sInstallLauncher += "ON"

	if not os.path.isdir("build"):
		os.mkdir("build")

	os.chdir("build")

	if not oArgs.bDontConfigure:
		subprocess.check_call("cmake {} {} {} {} {} {} ..".format(sBuildType, sInstallDir, sCMakeSchemasDir\
															, sInstallIcons, sInstallLauncher, sInstallMan).split())

	if not oArgs.bDontMake:
		subprocess.check_call("make $STMM_MAKE_OPTIONS", shell=True)

	if not oArgs.bDontInstall:
		try:
			sEnvDestDir = os.environ["DESTDIR"]
		except KeyError:
			sEnvDestDir = ""

		if sEnvDestDir != "":
			sEnvDestDir = "DESTDIR=" + sEnvDestDir
		subprocess.check_call("{} make install {}".format(sSudo, sEnvDestDir).split())

	if not oArgs.bDontCompileSchemas:
		subprocess.check_call("{} glib-compile-schemas {}/share/glib-2.0/schemas".format(sSudo, sSchemasDir).split())


if __name__ == "__main__":
	main()

