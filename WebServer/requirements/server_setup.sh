#!/bin/bash

# This script should only be run after a new user has been created

# Installing Postgres
sudo apt install python3-pip python3-dev libpq-dev postgresql postgresql-contrib build-essential libssl-dev libffi-dev python3-venv
sudo -u postgres createuser --interactive

sudo -u postgres createdb enitio2023treasurehunt

# Create SSH Credentials
ssh-keygen -t rsa -b 4096 -C "ENITIO2023 Server Key"
ssh-add ~/.ssh/id_rsa
echo "Please copy the public key below to GitHub to enable read access to the repository"
echo "Your Public Key is:"
cat ~/.ssh/id_rsa.pub
read -n 1 -s -r -p "Press any key to continue..."

# Clone the current web server code
git clone git@github.com:seemingyuan/ENITIO2023.git --config core.sshCommand="ssh -i ~/.ssh/id_rsa"

# Create a Python Virtual Environment
cd ~/ENITIO2023/WebServer
python3 -m venv venv
source venv/bin/activate

# Install Python dependencies
python3 -m pip install --upgrade pip
pip install pip-tools
cd requirements
pip-sync requirements.txt
# edit above statement to 'pip-sync requirements.txt dev-requirements.txt' in development
