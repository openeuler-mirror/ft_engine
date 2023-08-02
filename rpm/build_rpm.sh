SCRIPT_DIR=$(cd $(dirname $0);pwd)
PROJECT_DIR=$(dirname ${SCRIPT_DIR})

rpmdev-setuptree

cp -r ${PROJECT_DIR}/ ~/rpmbuild/SOURCES
cp ${PROJECT_DIR}/rpm/ft_engine.spec ~/rpmbuild/SPECS
mv -f ~/rpmbuild/SOURCES/ft_engine/ ~/rpmbuild/SOURCES/ft_engine-1.0
cd ~/rpmbuild/SOURCES/
tar -czvf ft_engine-1.0.tar.gz ft_engine-1.0
rm -rf ft_engine-1.0

rpmbuild -ba ~/rpmbuild/SPECS/ft_engine.spec
