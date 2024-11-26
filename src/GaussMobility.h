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

#ifndef __LINK_MAINTAIN_GAUSSMOBILITY_H
#define __LINK_MAINTAIN_GAUSSMOBILITY_H

#include "inet/mobility/base/LineSegmentsMobilityBase.h"
#include <omnetpp.h>

namespace inet
{

  /**
   * @brief Gauss  movement model. See NED file for more info.
   *
   * @author Marcin Kosiba
   */
  class GaussMobility : public LineSegmentsMobilityBase
  {
  protected:
    double speedX = 0.0;       ///< speed of the host
    double speedXMean = 0.0;   ///< speed mean
    double speedXStdDev = 0.0; ///< speed standard deviation
    double speedY = 0.0;       ///< speed of the host
    double speedYMean = 0.0;   ///< speed mean
    double speedYStdDev = 0.0; ///< speed standard deviation

    double margin = 0.0; ///< margin at which the host gets repelled from the border

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /** @brief Initializes mobility model parameters.*/
    virtual void initialize(int stage) override;

    /** @brief If the host is too close to the border it is repelled */
    void preventBorderHugging();

    /** @brief Move the host*/
    virtual void move() override;

    /** @brief Calculate a new target position to move to. */
    virtual void setTargetPosition() override;

  public:
    virtual double getMaxSpeed() const override { return sqrt(speedX * speedX + speedY * speedY); }
    virtual double getSpeedXMean() const { return speedXMean; }
    virtual double getSpeedXStdDev() const { return speedXStdDev; }
    virtual double getSpeedYMean() const { return speedYMean; }
    virtual double getSpeedYStdDev() const { return speedYStdDev; }
    virtual double getUpdateInterval() const { return updateInterval.dbl(); }
    GaussMobility();
  };

} // namespace inet

#endif
