cd /home/vagrant/NAO/NAO-engine/naoqi_modules/halagent
qibuild make -c crosscompiler
cd /home/vagrant/NAO/NAO-engine
scp naoqi_modules/halagent/build-crosscompiler/sdk/lib/naoqi/libhal_experimental.so nao@192.168.0.102:/home/nao/naoqi/lib
