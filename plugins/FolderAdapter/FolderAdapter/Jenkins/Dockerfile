FROM ubuntu:18.04

USER root
RUN apt-get update && apt-get install sudo -y && apt-get install git -y

RUN groupadd swuser && \
 useradd -m -d /home/swuser -g swuser swuser && \
 chown -R swuser:swuser /home/swuser && chmod 775 /home/swuser && \
 usermod -a -G sudo swuser && usermod -a -G swuser swuser && usermod -a -G root swuser
RUN mkdir -p /home/swuser/GitLab/FolderAdapter && chmod -R 775 /home/swuser && \
 mkdir -p /home/swuser/.ssh && chmod -R 600 /home/swuser/.ssh/
RUN echo 'swuser ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers
RUN cat /etc/sudoers
RUN cd /home/swuser/GitLab/FolderAdapter
COPY / /home/swuser/GitLab/FolderAdapter/
RUN chown -R swuser:swuser /home/swuser/GitLab/FolderAdapter && chmod 775 /home/swuser/GitLab/FolderAdapter
USER swuser
CMD ls -asl /home/swuser/GitLab/FolderAdapter/* && cd /home/swuser/GitLab/FolderAdapter && /bin/bash ./Jenkins/jenkinsbuildsetup.sh