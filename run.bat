@ECHO OFF
cd src
g++ -g *.cpp -o ..\build\app3.exe -O2 -ID:\entt\single_include -ID:\SFML-2.5.1\include -LD:\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
..\build\app3.exe
Rem g++ -static -static-libstdc++ -static-libgcc -g ..\*.cpp -o app.exe -O2 -ID:\SFML-2.5.1\include -LD:\SFML-2.5.1\lib  -lopengl32 -lwinmm -lgdi32 -lfreetype -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s
Rem g++ -g ..\*.cpp -o app.exe -O2 -ID:\SFML-2.5.1\include -LD:\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
