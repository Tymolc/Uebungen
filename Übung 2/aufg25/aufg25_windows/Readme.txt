Dieser Programmrahmen unterstützt x86 als auch x86_64 Zielumgebungen.


VCINSTALLDIR entspricht dem VC Verzeichnis der entsprechenden Visual Studio Version. 
Diese Variable ist in einer Visual Studio Cmd Umgebung gesetzt. Um die Variable in einer 'sauberen' CMD 
zu setzen, kann "%VS100COMNTOOLS%vsvars32.bat" (VS 2010, VS90COMNTOOLS für VS 2008) ausgeführt werden. 
Das setzt eine x86 Umgebung und ermöglicht auch die im weiteren beschrieben Umgebungswechsel.

z.B. C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC für VS 2008
     C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC für VS 2010
x86
---
> "%VCINSTALLDIR%\vcvarsall.bat x86"
> nmake


x64
---
Für eine x64 Binärdatei ist die Visual Studio x64 Umgebungen zu setzen

> "%VCINSTALLDIR%\vcvarsall.bat x64"

Beim Wechel zwischen beiden Umgebungen ist ein 

> nmake clean 

auszuführen um eine saubere Ausgangsbasis zu schaffen. 

Die Bibliothek fractal_XXX.dll hat 2 Implementierungen für getDescription:
 getDescriptionA   _MBCS    Multi-Byte Characters
 getDescriptionW   UNICODE  Unicode Characters
 In der Headerdatei algorithm.h wird anhand der gesetzten Präprozessorvariable A oder W 
 ausgewählt.
 
 Das Testprogramm gibt eine Warnung aus, wenn die Unicodeversion gebunden wird um auf 
 mögliche Fehler bei Speicherallokation und Rückgabe der Beschreibung hinzuweisen.