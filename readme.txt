1.spline control options
Let the user control the tension of the Catmull-Rom spline: Click "Some Bonus" button and then drag the tension slider.(Also support "Warp")
Implement one of the standard subdivision curves ( Lane-Riesenfeld ): It is in the choice memu of curves.(Also support "Warp")




2.general subdvision curves
In "subdivion" curve mode, click "Some Bonus" button and input the average mask in the text input(e.g. "0.5 0.5" or "-2 6 10 6 -2"). Press enter and enable "Normalized". 
Then click apply. Remember to let the number of control points be larger than the size of the mask.(Also support "Warp")



3.C2 interpolating curve	
It is in the choice memu of curves.(Also support "Warp")




4.quaternions
The rotating axis is perpendicular to the mouse movement.




5.edit CR curves
In CR curve mode, click "Some Bonus" button and enable "Add inner control". Click "Apply" button. You will see some extra controll points which is not on the curve.
You can drag those new points to control the curve.(Also support "Warp")



W spline control options			yes
(First two of them)
tension control
standard subdivision

W TGA						yes/no
W mirror					yes/no
W billboard					yes
W Fire						yes
+W snow						yes
+W firworks					yes
	turn on each of these by using the left sliders
W environment mapping				yes/no
W motion blur					yes/no
W Runge-Kutta					yes/no
W OpenGL Display list				yes/no
W de Casteljau					yes/no
B spring-mass system				yes
+B spring based cloth				yes
B collision detection AND handling		yes
	turn on the collision detection on the left slider pls, the effect can be seen from the smoke of the model
B general subdvision curves			yes
+B (might deserve)
B height fields					yes
B metaballs					yes/no
BW lens flare					yes/no
BW your great UI idea				yes/no
2B flocks					yes
	turn on with slider
2B C2 interpolating curve			yes
2B edit CR curves				yes
2B UI to select model parts by clicking		yes/no
2B quaternions					yes
2B bspline surface				yes
3B projected textures				yes
	turn on the projection texture slider on the left
3B motion warping				yes/no
3B cell shading					yes/no
4B rigid-body simulations 			yes
	turn on the left slider "regid body collision" and the simulation button,
	suggestion: turn off the cloth slider and use modeler window then it will be smoother
4B subdivision surfaces				yes/no
8B+ mouse/keyboard computer games (using IK)	yes/no