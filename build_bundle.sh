rm -rf InvGrid.app
mkdir -p InvGrid.app/Contents/MacOS
cp Info.plist InvGrid.app/Contents
mv InvGrid InvGrid.app/Contents/MacOS
macdeployqt InvGrid.app -dmg
