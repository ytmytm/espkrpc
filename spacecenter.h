
namespace KRPC {

namespace services {

const char SpaceCenterService[] = "SpaceCenter";

class Flight {
  public:
    Flight(uint32_t flightid) {
      flightArgument = new KRPC::Argument(0, new KRPC::pbBytes(flightid));
      args[0] = flightArgument;
    };
    ~Flight() {
      delete flightArgument;
    }
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
    ~Control() {
      delete controlArgument;
    }
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
    ~Orbit() {
      delete orbitArgument;
    }
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
    ~Resources() {
      delete resArgument;
    }
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
    ~AutoPilot() {
      delete autopilotArgument;
    }
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
    ~Vessel() {
      delete vesselArgument;
      delete ap;
      delete res;
      delete orb;
      delete ctr;
      delete flt;
    }
    Flight flight() {
      if (flt==NULL) {
        KRPC::Request rq(SpaceCenterService, "Vessel_Flight", args);
        uint32_t flightid;
        rq.getResponse(flightid);
        flt = new Flight(flightid);
      }
      return *flt;
    };
    Control control() {
      if (ctr==NULL) {
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Control", args);
        uint32_t controlid;
        rq.getResponse(controlid);
        ctr = new Control(controlid);
      }
      return *ctr;
    };
    Orbit orbit() {
      if (orb==NULL) {
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Orbit", args);
        uint32_t orbitid;
        rq.getResponse(orbitid);
        orb = new Orbit(orbitid);
      }
      return *orb;
    };
    Resources resources() {
      if (res==NULL) {
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Resources", args);
        uint32_t resourceid;
        rq.getResponse(resourceid);
        res = new Resources(resourceid);
      }
      return *res;
    };
    AutoPilot auto_pilot() {
      if (ap==NULL) {
        KRPC::Request rq(SpaceCenterService, "Vessel_get_AutoPilot", args);
        uint32_t autopilotid;
        rq.getResponse(autopilotid);
        ap = new AutoPilot(autopilotid);
      }
      return *ap;
    };
  private:
    AutoPilot *ap = NULL;
    Resources *res = NULL;
    Orbit *orb = NULL;
    Control *ctr = NULL;
    Flight *flt = NULL;
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

