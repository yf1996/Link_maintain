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

import link_maintain.INode;
import inet.mobility.contract.IMobility;
// TODO auto-generated module
//
//@namespace(inet);

module SimpleNode extends INode
{
    parameters:
    	@display("i=device/wifilaptop");
    	@class(SimpleNode);
        @signal[linkEnd](type=simtime_t);
        // @statistic[CountTest](title="Count Test"; source=countTest; record=timeavg, vector);	
    
        
        string radioMedium = default("^.radioMedium");
        int numSectors = default(4);
        double slotDuration @unit(s) = default(0.1s);
        double beaconDuration @unit(s) = default(0.1s);
        double commRange @unit(m) = default(200m);
        double sendInterval @unit(s) = default(2s);
        int linkStateThreshold = default(2);
    
        double listenningPower @unit(W) = default(0.1W);
        double receivingPower @unit(W) = default(0.05W);
        double transmittingPower @unit(W) = default(0.02W);
        
       	
        
    submodules:
        mobility: <default("StationaryMobility")> like IMobility {
           	@display("p=100,100;is=s");
        }
}
