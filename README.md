# BATTLESHIP

## COMMENT JOUER

Pour jouer au BattleShip il faut deux instances : un `server` et un `slient`.

Ouvir un terminal et se placer dans le répertoire du jeu. Ensuite l'instance `server` se placera dans le dossier `server` et l'instance `slient` dans le répertoire `client`.

Tout d'abord, vérifiez que vous possédez les droits d'exéction des fichier `client` et `server` :

```
ls -l
```

Pour donner les droits d'exécution (attention à être placé dans les répertoires respectifs des fichiers) :

```
sudo chmod 744 server
sudo chmod 744 client
```

### Lancer le jeu

Il faut d'abord lancer le serveur avec la commande :
```
./server [id] [port] [--showBoats]
```

- `id` : choisir un entier qui identifiera la partie
- `port` : choisir un port sur lequel le server écoutera
- `--showBoats` : permet d'afficher les cases des bateaux sur l'instance `server`

Si plus rien ne s'affiche dans le terminal c'est que le serveur est sur écoute donc ça fonctionne.

Ensuite, lancez l'instance `Client` avec cette commande :
```
./client [id] [adresse IP] [port]
```

- `id` : l'id de la partie choisi précédement
- `adresse IP` : adresse IP de la machine qui héberge le serveur
- `port` : le port d'écoute du serveur choisi précédement

> Pour connaître l'adresse IP de la machine du serveur, lancez la commande `ifconfig`. Si vous jouez sur une seule machine, l'adresse est `127.0.0.1`.

Si le client affiche un tableau alors la partie est lancée. Vous pouvez vérifier sur l'instance `Server` que le `Client` a bien rejoint.

Vous avez maitenant 50 essais pour trouver tous les bateaux. Pour toucher une case, entrez ses coordonnées séparées d'un tiret (exemple : 4-15, c'est la colonne 4, ligne 15).