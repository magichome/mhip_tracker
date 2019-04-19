# mhip_tracker : 3D Printed Parts

## Case and Solar Panel Mount

The ```cad``` folder has [SketchUp](https://www.sketchup.com/) and .STL files for the case (top and bottom) and solar panel mount.  These can be used to 3D print these components.

## Design Considerations

Packaging requirements:

- External mount
  - For solar power
  - Good gps singal reception
  - Good cellular connectivity.
- Self-contained
  - All of the parts together in a single package.

These requirements present several design considerations.  Since tracker is outside and moving with a vehicle, it is subject to hazards such as...

- Heat, rain, hail
- UV radiation
- Foreign object impact; e.g. rocks, tree limbs

Heat, rain, hail and UV radiation can be mitigated somewhat by keeping things shaded.  The solar panel cannot be shaded (obviously) but it is designed with the hazards in mind.  I can used the solar panel for shade it positioned off the vehicle enough.  Keep in mind though that solar panels get HOT.  So there would need to be good airflow around it so it doesn't transfer heat to the things it's shading.

Foreign object impacts are mitigated by keeping things as low profile as possible and designing them strong enough to handle incidental impacts.

### About the printer

I used the [Qidi X-Plus](http://www.qd3dprinter.com/products/x-plus/) printer.  I selected this printer based on reviews and max print size.  It has been very easy to use and reliable.  The slicing software seems a little less than smooth but it gets the job done.  The components are printed with generic PLA filament.  I chose translucent so I could see flashing lights on the components (for troubleshooting) without disassembly.

### Component Case

The case houses the Voltaic battery pack, the LiPo battery included with Boron, the Boron controller and the GPS board.  The battery pack being the largest of these.  I positioned the GPS board on top of the Boron so it's internal antenna would have an unobstructed view of the sky.  I positioned these toward one edge so I could position that side of the box exposed while the rest of the case could be in the shade under the solar panel.

### Solar Panel Mounts

These were designed to hold the panel up high enough to provide shade for the case.  In addition, they are somewhat minimal and open to improve airflow.  Finally, they fall along the short dimension of the panel to make them small enought to print in my 3D printer.

## Lessons Learned

Originally I had intended to mount the component case and solar panels separately.  I also intended to use heavy-duty double-sided tape to hold them to the vehicle.  I had already printed the cases with this in mind but quickly discovered the following...

_The box tabs are too small._

The tabs are too small to hold the case halves together.  For a "version 2" I would probably make these 2x larger and have four tabs, one along each side.

I also discovered that...

_The box warps as it cools, it won't lay flat._

This is a 3D printing thing for large, flat surfaces.  I think if I let it cool an the build platform it may stay flat.  In any case this means that using double-side tape to secure the case to the vehicle would be compromised since the contact surface area is reduced.

So while designing the solar mount brackets, I wanted to incorporate a way to tie the case to the brackets.  This would give a little extra force to keep it attached to the vehicle.

_The bracket "openings" for the box tabs are too tight._

3D print plastic expands a little during printing making holes and other clearances tighter than designed.  I allowed for this and it mostly worked out.  However, in this complex tab recepticle, it wasn't enough.  I took one shot at fixing the problem but it wasn't enough.  The brackets take a long time to print so instead I just used a dremel tool to open them up a bit.

_It's probably a good idea to get the nuts, bolts, and screws you intend to use before designing._

I sized holes according to screws and bolts I assumed I could readily find.  I couldn't.  That's why I went with plastic pins on the circuit boards and cable ties for the case tabs.  I think the approach I took turned out fine but I would have designed things a little differently if I knew this ahead of time.

_The case should probably be rotated 90 degrees counterclockwise so both batteries would be shaded by the solar panel._

Oooh, better idea.  Rotate the case _45 degrees_ counterclockwise!  That way only the corner with the GPS antenna needs to be outside the shade of the solar panel.  In addition the distance between the case and the solar panel mounts is reduced making for a more robust connection.  Finally, having the case angled to the vehicle motion would reduce the force of foreign object impacts.

### Closing Remarks

I've already printed all of the components I need for my application so I won't be spending any time right now to improve them.  If anyone would like to make improvements or offer alternative packaging and mounting designs, just give me a pull request.
