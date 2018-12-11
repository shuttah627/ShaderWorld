all: Week9Application

MOC=moc-qt5
moc_%.cpp: %.h
	$(MOC) -o moc_$*.cpp $*.h -I../ -I/usr/include/qt5 

%.o: %.cpp
	g++ -g -c -o $*.o -I../ -I/usr/include/qt5 -fPIC $*.cpp

OBJS=Week9Application.o Model.o  Shader.o  moc_Surface.o Surface.o    World.o LoadPNG.o
	
Week9Application: $(OBJS)
	g++ -g -o Week9Application $(OBJS) -lGLU -lGL -lQt5Gui -lQt5Core -lQt5OpenGL -lQt5Widgets 

clean:
	rm -f Week9Application $(OBJS) moc_Surface.cpp 
