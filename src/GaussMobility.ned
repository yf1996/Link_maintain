//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package link_maintain;

import inet.mobility.base.MovingMobilityBase;

//
// Uses a Gauss-Markov model to control the randomness in the movement.
// Totally random walk (Brownian motion) is obtained by setting alpha=0,
// while alpha=1 results a linear motion.
//
@namespace(inet);
simple GaussMobility extends MovingMobilityBase
{
    parameters:
        @class(GaussMobility);
        double initialX @unit(m) = default(uniform(constraintAreaMinX, constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(constraintAreaMinY, constraintAreaMaxY));
        double initialZ @unit(m) = default(nanToZero(uniform(constraintAreaMinZ, constraintAreaMaxZ)));

        // double alpha = default(0.5); // [0;1], 0 - brown motion, 1 - linear motion
        // double speed @unit(mps);
        // double speedStdDev @unit(mps);     // speed standard deviation
        // double angle @unit(deg) = default(uniform(0deg, 360deg));
        // double angleStdDev @unit(deg);     // angle standard deviation

        double speedX @unit(mps);
        double speedXStdDev @unit(mps);
        double speedY @unit(mps);
        double speedYStdDev @unit(mps);
        double margin @unit(m);
}
