#Tests

liste des fonctions :

+ initMsg
+ aboMsg
+ sendMsg
+ recvMsg
+ getNbMsg
+ desaboMsg
+ finMsg
+ isAbo / getAboList / getNbAbo / waitForAllAbo (?)

	
Codes d'erreurs :

+ 1 : Service déjà lancé (initMsg)
+ 2 : erreur d'initialisation (initMsg)
+ 3 : Service non lancé	(tous sauf initMsg)
+ 4 : Id déjà utilisé (aboMsg)
+ 5 : Nb max d'abonnés atteint (aboMsg)
+ 6 : Non abonné (sendMsg, RecvMsg, getNbMsg, desaboMsg)
+ 7 : Id inconnu (SendMsg)
+ 8 : Service toujours utilisé (finMsg)
+ 9 : Erreur technique (tous)

-----------------

##Tests à effectuer

+ appel de toutes les fonctions sauf initMsg, sans avoir démarrer le service.
+ appel de aboMsg avec un id déjà utilisé, ou avec le nb max d'abonnés atteint
+ appel de sendMsg, recvMsg, getNbMsg et desaboMsg sans être abonné (ou avec un faux id ?)
+ envoie d'un message a un abonné absent
+ appel de finMsg alors que le service est toujours utilisé,  
  puis appel de toutes les autres fonctions (par les autres threads clients).
+ test des comportements nominaux
