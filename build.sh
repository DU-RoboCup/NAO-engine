 #!/bin/bash
 
 # Build the main projects
 qibuild make -c linux64
 qibuild make -c cross-atom
 
 # Copy the config folder
 cp -R /home/vagrant/NAO/NAO-engine/config /home/vagrant/NAO/NAO-engine/build-cross-atom/sdk/bin/
 cp -R /home/vagrant/NAO/NAO-engine/config /home/vagrant/NAO/NAO-engine/build-linux64/sdk/bin/
 
 # Build the docs
 cd build-linux64
 echo "Building Documentation"
 make docs > /dev/null
 mv html/ docs/html
 mv latex/ docs/latex
 rm -rf ../docs/html
 rm -rf ../docs/latex
 mv docs/* ../docs/
