
cd..
cd..

xcopy .\Direct3DEngine\Include\*.h .\Engine\Include\ /d /y /s
xcopy .\Direct3DEngine\Include\*.hpp .\Engine\Include\ /d /y /s
xcopy .\Direct3DEngine\Bin\*.* .\Engine\Bin\ /d /y /s /exclude:exclude.txt
xcopy .\Direct3DEngine\Bin\*.* .\Client\Bin\ /d /y /s /exclude:exclude.txt
xcopy .\Direct3DEngine\Bin\*.* .\Editor\Bin\ /d /y /s /exclude:exclude.txt
xcopy .\Direct3DEngine\Bin\*.* .\Tool\Bin\ /d /y /s /exclude:exclude.txt

xcopy .\Editor\Bin\Mesh\*.* .\Client\Bin\Mesh\ /d /y /s /exclude:exclude.txt
xcopy .\Editor\Bin\Data\*.* .\Client\Bin\Data\ /d /y /s /exclude:exclude.txt

