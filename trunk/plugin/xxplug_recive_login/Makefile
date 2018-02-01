TARGET = ${XCZF_BIN}/dataserver/xxplug_recive_userlog.dll
INCLUDES = -I${XXLIB_ROOT}/include -I${MYSQL_ROOT}/include

CXXFLAGS = -g -Wall -fmessage-length=0 $(INCLUDES)
OBJS =	src/xxplug_recive_login.o
LIBS +=  -L${XXLIB_ROOT}/lib -L${MYSQL_ROOT}/lib -lmysql
$(TARGET): $(OBJS)
	$(CXX) -shared  -o $(TARGET) $(OBJS) $(LIBS) 

all:	$(TARGET)

clean:
	del -f $(OBJS) $(TARGET)
