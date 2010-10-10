rm -rf InvGrid.app
mkdir -p InvGrid.app/Contents/MacOS
mkdir -p InvGrid.app/Contents/Resources
cp Info.plist InvGrid.app/Contents
cp invgrid.icns InvGrid.app/Contents/Resources/
mv InvGrid InvGrid.app/Contents/MacOS
macdeployqt InvGrid.app -dmg
