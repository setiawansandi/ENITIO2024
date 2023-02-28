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
pip-sync requirements.txt production-requirements.txt

# Setup database TODO: make another script for this, since it requires editing the instance file

# Configure Gunicorn - Creating service
cd ..
read -p "Please enter system username: " username
cat > enitio2023.service << EOF
[Unit]
Description=ENITIO 2023 Treasure Hunt Web Server Gunicorn instance
After=network.target

[Service]
User=$username
Group=www-data
WorkingDirectory=$PWD
Environment=$PWD/venv/bin
ExecStart=$PWD/venv/bin/gunicorn --workers 3 --bind unix:enitio2023.sock -m 007 wsgi:app

[Install]
WantedBy=multi-user.target
EOF
sudo mv enitio2023.service /etc/systemd/system/enitio2023.service
sudo systemctl start enitio2023
sudo systemctl enable enitio2023

# Configuring Nginx
sudo apt install nginx
sudo ufw allow 'Nginx Full'
cat > enitio2023 << EOF
server {
    listen 80;
    server_name enitiotreasurehunt.link;

    location / {
        include proxy_params;
        proxy_pass http://unix:$PWD/enitio2023.sock;
    }
}

server {
    listen 443;
    server_name enitiotreasurehunt.link;

    location / {
        include proxy_params;
        proxy_pass https://unix:$PWD/enitio2023.sock;
    }
}
EOF
sudo mv enitio2023 /etc/nginx/sites-available/enitio2023
sudo ln -s /etc/nginx/sites-available/enitio2023 /etc/nginx/sites-enabled
sudo systemctl restart nginx

# We need to allow Nginx to access the gunicorn socket file
sudo chmod 755 /home/$username

# Setup HTTPS Certificate
sudo apt install python3-certbot-nginx
sudo certbot --nginx -d enitiotreasurehunt.link