makecert -n "CN=DogBase" -sk DogBaseKey -pe -sr localmachine -sky exchange -ss TRUST -r DogBase.cer
makecert -n "CN=DogServer" -sk DogServerKey -pe -sr localmachine -ss MY -sky exchange -ic DogBase.cer -is TRUST DogServer.cer
makecert -n "CN=DogClient" -sk DogClientKey -pe -sr localmachine -ss MY -sky exchange -ic DogBase.cer -is TRUST DogClient.cer