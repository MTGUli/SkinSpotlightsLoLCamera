--Test Script

--Functions:
--Print(Anything), Debug output only
--stopWatch:Start(), stopWatch:Stop(), Enable/Disable StopWatch
--stopWatch.tick, deltaTime essentially, needed to smooth everything
--stopWatch.enabled, Check to see if its enabled
--stopWatch.totalTick, amount of ticks (in ms, so 5000ms = 5 seconds) since it has started, note stopWatch.tick is needed to update the totalTick
--RotateRight, RotateUp, RotateDown, RotateLeft.  Takes 2 Params, stopWatch.tick and RotateSpeed, eg RotateRight(stopWatch.tick, 28.5)
--Position, 2 params, x and y, Bottom Left Corner map is 300, 300, top right is 14000, 14200
--MoveTowardsPosition, 4 Params, X, Y, Speed and stopWatch.tick
--Sleep, pauses it for how long in ms
--SetFoV, 1 param, float, sets field of view, default is 50

Print("Test");
stopWatch:Start();

while not MoveTowardsPosition(300, 300, 100, stopWatch.tick) do
end

Print("Done");

function testFunc()
Position(300, 300);
Sleep(4000);
Position(300, 2050);
Sleep(4000);
Position(300, 3800);
Sleep(4000);
Position(300, 5550);
Sleep(4000);
Position(300, 7300);
Sleep(4000);
Position(300, 9050);
Sleep(4000);
Position(300, 10800);
Sleep(4000);
Position(300, 12550);
Sleep(4000);
Position(300, 14300);

Position(3100, 300);
Sleep(4000);
Position(3100, 2050);
Sleep(4000);
Position(3100, 3800);
Sleep(4000);
Position(3100, 5550);
Sleep(4000);
Position(3100, 7300);
Sleep(4000);
Position(3100, 9050);
Sleep(4000);
Position(3100, 10800);
Sleep(4000);
Position(3100, 12550);
Sleep(4000);
Position(3100, 14300);

Position(5900, 300);
Sleep(4000);
Position(5900, 2050);
Sleep(4000);
Position(5900, 3800);
Sleep(4000);
Position(5900, 5550);
Sleep(4000);
Position(5900, 7300);
Sleep(4000);
Position(5900, 9050);
Sleep(4000);
Position(5900, 10800);
Sleep(4000);
Position(5900, 12550);
Sleep(4000);
Position(5900, 14300);

Position(11500, 300);
Sleep(4000);
Position(11500, 2050);
Sleep(4000);
Position(11500, 3800);
Sleep(4000);
Position(11500, 5550);
Sleep(4000);
Position(11500, 7300);
Sleep(4000);
Position(11500, 9050);
Sleep(4000);
Position(11500, 10800);
Sleep(4000);
Position(11500, 12550);
Sleep(4000);
Position(11500, 14300);

Position(14300, 300);
Sleep(4000);
Position(14300, 2050);
Sleep(4000);
Position(14300, 3800);
Sleep(4000);
Position(14300, 5550);
Sleep(4000);
Position(14300, 7300);
Sleep(4000);
Position(14300, 9050);
Sleep(4000);
Position(14300, 10800);
Sleep(4000);
Position(14300, 12550);
Sleep(4000);
Position(14300, 14300);
end

--Bottom corner 300, 300
--Top Corner 14000, 14200

--while stopWatch.totalTick < 5000 do
--		RotateRight(stopWatch.tick, 28.5);
--		Sleep(500);
--end
