$fn=150;

bodyWidth = 39.5;
bodyRadius = bodyWidth/2;

module sensorHolder() 
{
    holeWidth = 8;
    holeRadius = holeWidth/2;
    height = 20;
    
    translate([0,0,-140])
    difference() 
    {
        cylinder(h = height, r1 = bodyRadius, r2 = bodyRadius, center = true);
        cylinder(h = height, r1 = holeRadius, r2 = holeRadius, center = true);
    }
}

module batteryHolder() 
{
    height = 58;
    
    difference() 
    {
        difference() 
        {
            cylinder(h = height, r1 = bodyRadius, r2 = bodyRadius, center = true);
            cylinder(h = height, r1 = bodyRadius-2, r2 = bodyRadius-2, center = true);
        }
        
        translate([0,19,30])
            cube(size = [40, 48, height], center = true);
    }   
    
    difference() 
    {
        cylinder(h = height, r1 = bodyRadius-2, r2 = bodyRadius-2, center = true);
        translate([0,15,0])
            cube(size = [40, 40, height], center = true);
    }
    
}

module pvcPipe() 
{
    height = 300;
    outerWidth = 48.5;
    innerWidth = 40;
    
   difference() 
    {
        cylinder(h = height, r1 = outerWidth/2, r2 = outerWidth/2, center = true);
        cylinder(h = height, r1 = innerWidth/2, r2 = innerWidth/2, center = true);
    }
}

module float(notch)
{
    translateVal = notch ? 100 : -100;
    difference()
    {
        difference()
        {
            floatBase();
            translate([translateVal,0,0])
            cube(size = [200, 200, 200], center = true);
        }
        translate([translateVal,0,0])
        cube(size = [200, 200, 200], center = true);
    }
}

module floatBase()
{
    topWidth = 57.5;
    bottomWidth = 49.6;
    topHeight = 28.5;
    bottomHeight = 50;
    outerWidth = 140;
    
    difference() 
    {
        cube(size = [6, outerWidth, topHeight + bottomHeight], center = true);
        cylinder(h = 80, r1 = topWidth/2, r2 = topWidth/2, center = true);
    }
    
    difference() 
    {
        difference() 
        {
            difference() 
            {
                difference() 
                {
                    cylinder(h = topHeight + bottomHeight, r1 = outerWidth/2, r2 = outerWidth/2, center = true);
                    
                    translate([0,0,25])
                    cylinder(h = topHeight, r1 = topWidth/2, r2 = topWidth/2, center = true);
                }
                
                translate([0,0,-14])
                cylinder(h = bottomHeight+5, r1 = bottomWidth/2, r2 = bottomWidth/2, center = true);
            }
            translate([0,0,3])
            floatChamber();
        }
        translate([-30 ,0, 37.25])
        cube(size = [6, 10, 5], center = true);
    }
    
    notch();

}

module floatLid(notch)
{
    translateVal = notch ? 100 : -100;
    
    difference()
    {
        difference()
        {
            floatLidBase();
            translate([translateVal,0,0])
            cube(size = [200, 200, 200], center = true);
        }
        translate([translateVal,0,0])
        cube(size = [200, 200, 200], center = true);
    }
}

module floatLidBase()
{
    topWidth = 57.5;
    bottomWidth = 49.6;
    topHeight = 4;
    bottomHeight = 50;
    outerWidth = 140;
    
    difference() 
    {
        cube(size = [6, outerWidth, topHeight], center = true);
        cylinder(h = 80, r1 = topWidth/2, r2 = topWidth/2, center = true);
    }
    
    difference() 
    {
        difference() 
        {
            cylinder(h = topHeight, r1 = outerWidth/2, r2 = outerWidth/2, center = true);
            
            translate([0,0,0])
            cylinder(h = topHeight, r1 = topWidth/2, r2 = topWidth/2, center = true);
        }
        
        translate([-32 ,0, 0])
        cube(size = [10, 10, 5], center = true);
    }
}

module floatChamber()
{
    outerWidth = 134;
    height = 72.5;
    difference() 
    {
        cylinder(h = height, r1 = outerWidth/2, r2 = outerWidth/2, center = true);
        cylinder(h = height, r1 = 32, r2 = 32, center = true);
    }
}

module notch()
{
    height = 33.25;
    translate([-17,0,0])    
    difference()
    {
        translate([-20,0,height])
        difference()
        {
            cube(size = [12, 15, 12], center = true);
            translate([-10,0,-10])
            rotate([0, 45, 0])
            cube(size = [30, 30, 30], center = true);
        }
        translate([-17,0,height+4])
        cube(size = [6, 10, 5], center = true);
    }
}




//floatLid(true); //Print if your PVC has a notch

//floatLid(false); //Print two if your PVC does not have a notch

//pvcPipe();

//float(true); //Print if your PVC has a notch

//floast(false) //Print two if your PVC does not have a notch

//floatChamber();

//sensorHolder(); //Used to hold temp sensor (modify height based on your PVC Length)

//batteryHolder(); //glued to temp sensor holder



