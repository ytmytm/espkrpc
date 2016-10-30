
namespace KRPC {

namespace services {

const char SpaceCenterService[] = "SpaceCenter";

class Flight {
  public:
    Flight(uint32_t flightid) {
      flightArgument = new KRPC::Argument(0, new KRPC::pbBytes(flightid));
      args[0] = flightArgument;
    };
    double mean_altitude() {
      KRPC::Request rq(SpaceCenterService, "Flight_get_MeanAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
    double surface_altitude() {
      KRPC::Request rq(SpaceCenterService, "Flight_get_SurfaceAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *flightArgument;
    KRPC::Argument *args[2] = { NULL, NULL };
};

class Control {
  public:
    Control(uint32_t controlid) {
      controlArgument = new KRPC::Argument(0, new KRPC::pbBytes(controlid));
      args[0] = controlArgument;
      args2[0] = controlArgument;
    };
    void activate_next_stage() {
      KRPC::Request rq(SpaceCenterService, "Control_ActivateNextStage", args);
      rq.getResponse();
    };
    float throttle() {
      KRPC::Request rq(SpaceCenterService, "Control_get_Throttle", args);
      float val;
      rq.getResponse(val);
      return val;
    };
    void throttle(float thr) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(thr));
      args2[1] = &valueArgument;
      KRPC::Request rq(SpaceCenterService, "Control_set_Throttle", args2);
      rq.getResponse();
    };
  private:
    KRPC::Argument *controlArgument;
    KRPC::Argument *args[2] = { NULL, NULL };
    KRPC::Argument *args2[3] = { NULL, NULL, NULL };
};

class Orbit {
  public:
    Orbit(uint32_t orbitid) {
      orbitArgument = new KRPC::Argument(0, new KRPC::pbBytes(orbitid));
      args[0] = orbitArgument;
    };
    double apoapsis_altitude() {
      KRPC::Request rq(SpaceCenterService, "Orbit_get_ApoapsisAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *orbitArgument;
    KRPC::Argument *args[2] = { NULL, NULL };
};

class Resources {
  public:
    Resources(uint32_t resid) {
      resArgument = new KRPC::Argument(0, new KRPC::pbBytes(resid));
      args[0] = resArgument;
    };
    float amount(char *resource_name) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(resource_name));
      args[1] = &valueArgument;
      KRPC::Request rq(SpaceCenterService, "Resources_Amount", args);
      float val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *resArgument;
    KRPC::Argument *args[3] = { NULL, NULL, NULL };
};

class AutoPilot {
  public:
    AutoPilot(uint32_t autopilotid) {
      autopilotArgument = new KRPC::Argument(0, new KRPC::pbBytes(autopilotid));
      args[0] = autopilotArgument;
      args2[0] = autopilotArgument;
      args3[0] = autopilotArgument;
    };
    void engage() {
      KRPC::Request rq(SpaceCenterService, "AutoPilot_Engage", args);
      rq.getResponse();
    };
    void disengage() {
      KRPC::Request rq(SpaceCenterService, "AutoPilot_Disengage", args);
      rq.getResponse();
    };
    void set_target_pitch(float value) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(value));
      args2[1] = &valueArgument;
      KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetPitch", args2);
      rq.getResponse();
    };
    void set_target_heading(float value) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(value));
      args2[1] = &valueArgument;
      KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetHeading", args2);
      rq.getResponse();
    };
    void set_target_pitch_and_heading(float pitch, float heading) {
      KRPC::Argument valueArgument1 = KRPC::Argument(1, new KRPC::pbBytes(pitch));
      KRPC::Argument valueArgument2 = KRPC::Argument(2, new KRPC::pbBytes(heading));
      args3[1] = &valueArgument1;
      args3[2] = &valueArgument2;
      KRPC::Request rq(SpaceCenterService, "AutoPilot_TargetPitchAndHeading", args3);
      rq.getResponse();
    };
  private:
    KRPC::Argument *autopilotArgument;
    KRPC::Argument *args[2] = { NULL, NULL };
    KRPC::Argument *args2[3] = { NULL, NULL, NULL };
    KRPC::Argument *args3[4] = { NULL, NULL, NULL, NULL };
};

class Vessel {
  public:
    Vessel(uint32_t vesselid) {
      vesselArgument = new KRPC::Argument(0, new KRPC::pbBytes(vesselid));
      args[0] = vesselArgument;
    };
    Flight flight() {
      KRPC::Request rq(SpaceCenterService, "Vessel_Flight", args);
      rq.getResponse(flightid);
      return Flight(flightid);
    };
    Control control() {
      KRPC::Request rq(SpaceCenterService, "Vessel_get_Control", args);
      rq.getResponse(controlid);
      return Control(controlid);
    };
    Orbit orbit() {
      KRPC::Request rq(SpaceCenterService, "Vessel_get_Orbit", args);
      rq.getResponse(orbitid);
      return Orbit(orbitid);
    };
    Resources resources() {
      KRPC::Request rq(SpaceCenterService, "Vessel_get_Resources", args);
      rq.getResponse(resourceid);
      return Resources(resourceid);
    };
    AutoPilot auto_pilot() {
      KRPC::Request rq(SpaceCenterService, "Vessel_get_AutoPilot", args);
      rq.getResponse(autopilotid);
      return AutoPilot(autopilotid);
    };
  private:
    uint32_t flightid, controlid, orbitid, resourceid, autopilotid;
    KRPC::Argument *vesselArgument;
    KRPC::Argument *args[2] = { NULL, NULL };
};

class SpaceCenter {
  public:
    SpaceCenter() { };
    Vessel active_vessel(void) {
      uint32_t val;
      KRPC::Request rq(SpaceCenterService, "get_ActiveVessel");
      rq.getResponse(val);
      return Vessel(val);
    }
} SpaceCenter;
}
}

