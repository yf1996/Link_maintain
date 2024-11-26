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

#include "GaussMobility.h"

namespace inet
{

    Define_Module(GaussMobility);

    GaussMobility::GaussMobility()
    {
    }

    void GaussMobility::initialize(int stage)
    {
        LineSegmentsMobilityBase::initialize(stage);

        EV_TRACE << "initializing GaussMobility stage " << stage << endl;
        if (stage == INITSTAGE_LOCAL)
        {
            speedXMean = par("speedX");
            speedXStdDev = par("speedXStdDev");
            speedYMean = par("speedY");
            speedYStdDev = par("speedYStdDev");
            margin = par("margin");

            speedX = speedXMean;
            speedY = speedYMean;
            stationary = (speedX == 0 && speedY == 0);
        }
    }

    void GaussMobility::preventBorderHugging()
    {
        bool left = (lastPosition.x < constraintAreaMin.x + margin);
        bool right = (lastPosition.x >= constraintAreaMax.x - margin);
        bool top = (lastPosition.y < constraintAreaMin.y + margin);
        bool bottom = (lastPosition.y >= constraintAreaMax.y - margin);
        if (top || bottom)
        {
            speedYMean = -speedYMean;
        }
        if (left || right)
        {
            speedXMean = -speedXMean;
        }
    }

    void GaussMobility::move()
    {
        preventBorderHugging();
        LineSegmentsMobilityBase::move();
        rad angle = rad(atan2(speedY, speedX));
        handleIfOutside(REFLECT, lastPosition, lastVelocity, angle);
    }

    void GaussMobility::setTargetPosition()
    {
        // calculate new speed and direction based on the model
        speedX = speedXMean + normal(0.0, 1.0) * speedXStdDev;
        speedY = speedYMean + normal(0.0, 1.0) * speedYStdDev;

        // angle = alpha * angle + (1.0 - alpha) * angleMean + rad(sqrt(1.0 - alpha * alpha) * normal(0.0, 1.0) * angleStdDev);

        Coord direction(speedX, speedY);
        nextChange = simTime() + updateInterval;
        targetPosition = lastPosition + direction * updateInterval.dbl();

        EV_DEBUG << " speedX = " << speedX << " speedY = " << speedY << endl;
        EV_DEBUG << " mspeedY = " << speedXMean << " mspeedY = " << speedYMean << endl;
    }

} // namespace inet
