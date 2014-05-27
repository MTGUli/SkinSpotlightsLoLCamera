SkinSpotlightsLoLCamera
=======================

Custom LoL Camera created in order to improve my Spotlight videos over at youtube.com/SkinSpotlights

To contact me, message me through youtube or message me through my Business Inquiries Email, that is located in the about page of my youtube channel.


Hotkeys:
Numpad1 = Decrease Rotation Speed

Numpad3 = Increase Rotation Speed

Numpad4 = Rotate Left

Numpad6 = Rotate Right

Numpad5 = Reset FOV + All Rotations

Numpad2 = Rotate Down

Numpad8 = Rotate Up

Numpad7 = Decrease FoV

Numpad9 = Increase FoV


Numpad - = Lower Camera

Numpad + = Raise Camera


Numpad0 = Run LUA Script

Mouse:

Hold down Left + Right Click and move the mouse to rotate Camera.


LUA Scripting Functions:

Print(Anything), Debug output only

stopWatch:Start(), stopWatch:Stop(), Enable/Disable StopWatch

stopWatch.tick, deltaTime essentially, needed to smooth everything

stopWatch.enabled, Check to see if its enabled

stopWatch.totalTick, amount of ticks (in ms, so 5000ms = 5 seconds) since it has started, note stopWatch.tick is needed to update the totalTick

RotateRight, RotateUp, RotateDown, RotateLeft.  Takes 2 Params, stopWatch.tick and RotateSpeed, eg RotateRight(stopWatch.tick, 28.5)

Position, 2 params, x and y, Bottom Left Corner map is 300, 300, top right is 14000, 14200

MoveTowardsPosition, 4 Params, X, Y, Speed and stopWatch.tick

Sleep, pauses it for how long in ms

SetFoV, 1 param, float, sets field of view, default is 50
