# IVS - 2. project Makefile
# Author - Marek Nesvadba (xnesva06)

LOGINS = xnesva06_xdolez82_xgajdo22_xmacha73

all:	# Compiles program and profiling - as there is nothing to compile in python, this will install required packages instead Python3 should be already installed
	echo "Installing pip and required packages"
	sudo apt-get -y update
	sudo apt install python3-pip
	pip3 install -r requirements.txt
	
pack: clean doc	# Packages project for submission
	mkdir ../$(LOGINS)
	mkdir ../$(LOGINS)/install
	mkdir ../$(LOGINS)/repo
	mv ../doc ../$(LOGINS)
	rsync -r --exclude='$(LOGINS)' ../ ../$(LOGINS)/repo

	zip -r ../$(LOGINS).zip ../$(LOGINS)
	rm -r -f ../$(LOGINS)
	
clean:	# Deletes files that should not be submitted
	rm -r -f ../$(LOGINS)
	rm -r -f ../doc
	$(find .. -name __pycache__ -exec rm -rf {} \; > /dev/null 2>&1)

test: matlib_tests.py	# Runs tests of math library
	python3 matlib_tests.py

doc: Doxyfile	# Generates code documentation
	doxygen

run: main.py	# Runs program
	python3 main.py

profile:	# Compiles profiling - nothing to complile in python
	echo "To start profiling run stddev.py with data on stdin"
