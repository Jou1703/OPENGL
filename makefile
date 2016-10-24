
EXECUTABLE:=SteamUI
CC=g++ 


LIBS := glut GLU GL m pthread asound xcb freetype jpeg png
CFLAGS := -g -Wall -O3 
CXXFLAGS := $(CFLAGS) 


SOURCE := $(wildcard *.cpp ./alsa_src/*.cpp)  
OBJS := $(patsubst %.cpp,%.o,$(patsubst %.cpp,%.o,$(SOURCE))) 
DEPS := $(patsubst %.o,%.d,$(OBJS)) 
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS)) 
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS))) 
CPPFLAGS += -MD 

dest : $(OBJS) 
	$(CC) -I -c  $(SOURCE) $(addprefix -l,$(LIBS))
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))
	chmod u+s $(EXECUTABLE)	

.PHONY:clean	
clean:
	rm $(EXECUTABLE) alsa_src/*.o alsa_src/*.d *.o *.d *~ -rf 

	
  
 


	
