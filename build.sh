 #!/bin/bash
 qibuild make -c linux64
 qibuild make -c cross-atom
 cd build-linux64
 make docs
 mv html/ docs/html
 mv latex/ docs/latex
 rm -rf ../docs/html
 rm -rf ../docs/latex
 mv docs/* ../docs/
