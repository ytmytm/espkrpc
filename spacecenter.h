
#include <vector>

namespace KRPC {

namespace services {

const char SpaceCenterService[] = "SpaceCenter";

class Flight {
  public:
    Flight(uint32_t flightid) {
      flightArgument = new KRPC::Argument(0, new KRPC::pbBytes(flightid));
    };
    ~Flight() {
      delete flightArgument;
    }
    double mean_altitude() {
      std::vector<KRPC::Argument*> args = { flightArgument };
      KRPC::Request rq(SpaceCenterService, "Flight_get_MeanAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
    double surface_altitude() {
      std::vector<KRPC::Argument*> args = { flightArgument };
      KRPC::Request rq(SpaceCenterService, "Flight_get_SurfaceAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *flightArgument;
};

class Control {
  public:
    Control(uint32_t controlid) {
      controlArgument = new KRPC::Argument(0, new KRPC::pbBytes(controlid));
    };
    ~Control() {
      delete controlArgument;
    }
    void activate_next_stage() {
      std::vector<KRPC::Argument*> args = { controlArgument };
      KRPC::Request rq(SpaceCenterService, "Control_ActivateNextStage", args);
      rq.getResponse();
    };
    float throttle() {
      std::vector<KRPC::Argument*> args = { controlArgument };
      KRPC::Request rq(SpaceCenterService, "Control_get_Throttle", args);
      float val;
      rq.getResponse(val);
      return val;
    };
    void throttle(float value) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(value));
      std::vector<KRPC::Argument*> args = { controlArgument, &valueArgument };
      KRPC::Request rq(SpaceCenterService, "Control_set_Throttle", args);
      rq.getResponse();
    };
  private:
    KRPC::Argument *controlArgument;
};

class Orbit {
  public:
    Orbit(uint32_t orbitid) {
      orbitArgument = new KRPC::Argument(0, new KRPC::pbBytes(orbitid));
    };
    ~Orbit() {
      delete orbitArgument;
    }
    double apoapsis_altitude() {
      std::vector<KRPC::Argument*> args = { orbitArgument };
      KRPC::Request rq(SpaceCenterService, "Orbit_get_ApoapsisAltitude", args);
      double val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *orbitArgument;
};

class Resources {
  public:
    Resources(uint32_t resid) {
      resArgument = new KRPC::Argument(0, new KRPC::pbBytes(resid));
    };
    ~Resources() {
      delete resArgument;
    }
    float amount(char *resource_name) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(resource_name));
      std::vector<KRPC::Argument*> args = { resArgument, &valueArgument };
      KRPC::Request rq(SpaceCenterService, "Resources_Amount", args);
      float val;
      rq.getResponse(val);
      return val;
    };
  private:
    KRPC::Argument *resArgument;
};

class AutoPilot {
  public:
    AutoPilot(uint32_t autopilotid) {
      autopilotArgument = new KRPC::Argument(0, new KRPC::pbBytes(autopilotid));
    };
    ~AutoPilot() {
      delete autopilotArgument;
    }
    void engage() {
      std::vector<KRPC::Argument*> args = { autopilotArgument };
      KRPC::Request rq(SpaceCenterService, "AutoPilot_Engage", args);
      rq.getResponse();
    };
    void disengage() {
      std::vector<KRPC::Argument*> args = { autopilotArgument };
      KRPC::Request rq(SpaceCenterService, "AutoPilot_Disengage", args);
      rq.getResponse();
    };
    void set_target_pitch(float value) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(value));
      std::vector<KRPC::Argument*> args = { autopilotArgument, &valueArgument };
      KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetPitch", args);
      rq.getResponse();
    };
    void set_target_heading(float value) {
      KRPC::Argument valueArgument = KRPC::Argument(1, new KRPC::pbBytes(value));
      std::vector<KRPC::Argument*> args = { autopilotArgument, &valueArgument };
      KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetHeading", args);
      rq.getResponse();
    };
    void set_target_pitch_and_heading(float pitch, float heading) {
      KRPC::Argument valueArgument1 = KRPC::Argument(1, new KRPC::pbBytes(pitch));
      KRPC::Argument valueArgument2 = KRPC::Argument(2, new KRPC::pbBytes(heading));
      std::vector<KRPC::Argument*> args = { autopilotArgument, &valueArgument1, &valueArgument2 };
      KRPC::Request rq(SpaceCenterService, "AutoPilot_TargetPitchAndHeading", args);
      rq.getResponse();
    };
  private:
    KRPC::Argument *autopilotArgument;
};

class Vessel {
  public:
    Vessel(uint32_t vesselid) {
      vesselArgument = new KRPC::Argument(0, new KRPC::pbBytes(vesselid));
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
        std::vector<KRPC::Argument*> args = { vesselArgument };
        KRPC::Request rq(SpaceCenterService, "Vessel_Flight", args);
        uint32_t flightid;
        rq.getResponse(flightid);
        flt = new Flight(flightid);
      }
      return *flt;
    };
    Control control() {
      if (ctr==NULL) {
        std::vector<KRPC::Argument*> args = { vesselArgument };
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Control", args);
        uint32_t controlid;
        rq.getResponse(controlid);
        ctr = new Control(controlid);
      }
      return *ctr;
    };
    Orbit orbit() {
      if (orb==NULL) {
        std::vector<KRPC::Argument*> args = { vesselArgument };
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Orbit", args);
        uint32_t orbitid;
        rq.getResponse(orbitid);
        orb = new Orbit(orbitid);
      }
      return *orb;
    };
    Resources resources() {
      if (res==NULL) {
        std::vector<KRPC::Argument*> args = { vesselArgument };
        KRPC::Request rq(SpaceCenterService, "Vessel_get_Resources", args);
        uint32_t resourceid;
        rq.getResponse(resourceid);
        res = new Resources(resourceid);
      }
      return *res;
    };
    AutoPilot auto_pilot() {
      if (ap==NULL) {
        std::vector<KRPC::Argument*> args = { vesselArgument };
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

