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

//
// TODO auto-generated module
//
//@namespace(inet);
simple RadioMedium
{
    @class(RadioMedium);
    
    @signal[currentLinkNum](type=int);
    @signal[lostNode](type=int);
    @statistic[CurrentLinkNum](title="Current Link Number"; source=currentLinkNum; record=vector);
    @statistic[LostNodeNum](title="Lost Node Number"; source=lostNode; record=sum, vector);	
    
    double constraintAreaMinX @unit(m) = default(0m); // min x position of the constraint area
    double constraintAreaMinY @unit(m) = default(0m); // min y position of the constraint area
    double constraintAreaMinZ @unit(m) = default(0m); // min z position of the constraint area
    double constraintAreaMaxX @unit(m) = default(0m); // max x position of the constraint area
    double constraintAreaMaxY @unit(m) = default(0m); // max y position of the constraint area
    double constraintAreaMaxZ @unit(m) = default(0m); // max z position of the constraint area
}
