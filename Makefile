CXX=g++

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		ip_search.o AddressTable.o

LIBS =

TARGET =	ip_search.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) prefs.txt
