cd /home/user
rm -r .local/share/flashmaps/maps
mkdir .local/share/flashmaps/maps
cd flashmaps/build-main-Desktop-Debug/pan2flash

./pan2flash /home/user/flashmaps/src/class admin0.json /home/user/flashmaps/worldbase/admin0 /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class admin1.json /home/user/flashmaps/worldbase/admin1 /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class rivers.json /home/user/flashmaps/worldbase/rivers /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class lakes.json /home/user/flashmaps/worldbase/lakes /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class roads.json /home/user/flashmaps/worldbase/roads /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class boundaries.json /home/user/flashmaps/worldbase/boundaries /home/user/.local/share/flashmaps/maps/global
./pan2flash /home/user/flashmaps/src/class global.json /home/user/flashmaps/panmaps /home/user/.local/share/flashmaps/maps/global

cd /home/user/flashmaps/build-main-Desktop-Debug/flashunite
./flashunite /home/user/.local/share/flashmaps/maps/global ru-spe,ru-len.flashmap /home/user/.local/share/flashmaps/maps/world.flashmap
