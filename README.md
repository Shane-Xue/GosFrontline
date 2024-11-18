# GosFrontline Renju program

## About this program
This program is an implementation of Renju. It allows pvp playing and pve playing. It checks against unallowed moves for the black (sente) side, including:

- 3\-3 bans (Sente unallowed to play a move creating 2 live-3s)
- 4-4 bans (Sente unallowed to play a move creating 2 live-4s)
- overflow bans (Sente unallowed to create a consecutive row of more than 5 stones)

Notice 3-4 is also a ban which is included in the 3-3 code.

## About the Name
On October 31, 2024, the MICA team announced that Girls' Frontline would shut down it's mainland servers after 8 years of service. Since Renju's pieces have a resemblance to go and Go's Frontline sounds like Girls' Frontline, I sickly gave this name to this project.
