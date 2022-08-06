HOST=${NODE}
APP_DIR=/root/RF_HW_INTF


TARGET_EXEC ?= my_pid_app

BUILD_DIR ?= ./build
SRC_DIRS ?= ./

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -Ofast --fast-math -finline-functions -std=c++11


LIBDIR=-L/usr/local/lib/x86_64-linux-gnu
LDFLAGS=-lboost_program_options -lboost_system -lboost_thread -lpthread
#LDFLAGS=-lboost_program_options -lboost_system -lboost_thread -lpthread -lfftw3f -lrt -luhd


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) *.o

-include $(DEPS)

MKDIR_P ?= mkdir -p


test1 :
	./build/my_pid_app --kp 1.1 --ki 0.2 --kd .02 --sv 4.2 --iter 38 --assert_value -10530.966796875

test2 :
	./build/my_pid_app --kp 0.1 --ki 0.2 --kd .02 --sv 4.2 --iter 400 --assert_value 4.2

test3 :
	./build/my_pid_app --kp 0.125 --ki 0.25 --kd .0625 --sv 4.2 --iter 80 --assert_value 4.2


remote-make :
	ssh root@$(HOST) 'mkdir -p $(APP_DIR)'
	rsync -ptr *      root@$(HOST):$(APP_DIR)
	ssh root@$(HOST) 'cd $(APP_DIR) && make -j 4'
#	ssh root@$(HOST) 'cd $(APP_DIR) && mv build/rf_hw_intf /usr/local/bin/.'

remote-clean :
	ssh root@$(HOST) 'cd $(APP_DIR) && make clean'

remote-chlorox :
	ssh root@$(HOST) 'rm -rf $(APP_DIR)'



remote-install-websocket :
	ssh root@$(HOST) 'git clone https://github.com/zaphoyd/websocketpp.git'
	ssh root@$(HOST) 'cd websocketpp; cmake . -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF; make -j 4; make install'
	ssh root@$(HOST) 'rm -rf websocketpp'



remote-install-deps :
	ssh root@$(HOST) 'rm -f /etc/apt/sources.list.d/mellanox.list'
	ssh root@$(HOST) 'apt update'
#	ssh root@$(HOST) 'apt -y install zlib1g-dev libssl-dev'
#	ssh root@$(HOST) 'apt -y install libpugixml1 libpugixml-dev'
#	ssh root@$(HOST) 'apt -y install libboost-all-dev'
	ssh root@$(HOST) 'apt -y install libfftw3-dev'
#	ssh root@$(HOST) 'apt -y install net-tools cmake'
#	ssh root@$(HOST) 'apt -y install liboml2 liboml2-dev liblog4cxx-dev octave'

remote-config-eth1 :
	ssh root@$(HOST) 'ifconfig eth1 mtu 9000'
	ssh root@$(HOST) 'sysctl -w net.core.rmem_max=50000000'
	ssh root@$(HOST) 'sysctl -w net.core.wmem_max=2500000'

remote-config-eth2 :
	ssh root@$(HOST) 'ifconfig eth2 192.168.10.1 mtu 8000'
	ssh root@$(HOST) 'sysctl -w net.core.rmem_max=50000000'
	ssh root@$(HOST) 'sysctl -w net.core.wmem_max=5000000'

remote-config-netw :
	ssh root@$(HOST) 'ip -c link set enp3s0 down'
	ssh root@$(HOST) 'ip -c link set enp3s0 up'
	ssh root@$(HOST) 'ip -c a add 192.168.10.1/24 dev enp3s0'
	ssh root@$(HOST) 'ip -c link set enp3s0 mtu 8000'
	ssh root@$(HOST) 'sysctl -w net.core.rmem_max=50000000'
	ssh root@$(HOST) 'sysctl -w net.core.wmem_max=1048576'
