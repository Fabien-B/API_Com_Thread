#Tests

liste des fonctions :

+ initMsg
+ aboMsg
+ sendMsg
+ recvMsg
+ getNbMsg
+ desaboMsg
+ finMsg
	
Codes d'erreurs :

+ 1 : Service déjà lancé (initMsg)
+ 2 : erreur d'initialisation (initMsg)
+ 3 : Service non lancé	(tous sauf initMsg)
+ 4 : Id déjà utilisé (aboMsg)
+ 5 : Nb max d'abonnés atteint (aboMsg)
+ 6 : Erreur technique (tous)
+ 7 : Non abonné (sendMsg, RecvMsg, getNbMsg, desaboMsg)
+ 8 : Id inconnu (SendMsg)
+ 9 : Service toujours utilisé (finMsg)

-----------------

##Tests à effectuer

+ appel de toutes les fonctions sauf initMsg, sans avoir démarrer le service.
+ 
