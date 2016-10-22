
const char SpaceCenterService[] = "SpaceCenter";

namespace KRPC {
  // vessel
  uint32_t active_vessel(void) {
    KRPC::Request rq(SpaceCenterService, "get_ActiveVessel");
    sendRequest(rq);
    return getIntResponse();
  }
  // orbit
  uint32_t orbit(uint32_t active_vessel) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(active_vessel)), NULL };
    KRPC::Request rq(SpaceCenterService, "Vessel_get_Orbit", args);
    sendRequest(rq);
    return getIntResponse();
  }
  double orbit_get_ApoapsisAltitude(uint32_t vessel_orbit) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_orbit)), NULL };
    KRPC::Request rq(SpaceCenterService, "Orbit_get_ApoapsisAltitude", args);
    sendRequest(rq);
    return getDoubleResponse();
  }
  // flight
  uint32_t flight(uint32_t active_vessel) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(active_vessel)), NULL };
    KRPC::Request rq(SpaceCenterService, "Vessel_Flight", args);
    sendRequest(rq);
    return getIntResponse();
  }
  double flight_get_MeanAltitude(uint32_t vessel_flight) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_flight)), NULL };
    KRPC::Request rq(SpaceCenterService, "Flight_get_MeanAltitude", args);
    sendRequest(rq);
    return getDoubleResponse();
  }
  double flight_get_SurfaceAltitude(uint32_t vessel_flight) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_flight)), NULL };
    KRPC::Request rq(SpaceCenterService, "Flight_get_SurfaceAltitude", args);
    sendRequest(rq);
    return getDoubleResponse();
  }
  // resources
  uint32_t resources(uint32_t active_vessel) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(active_vessel)), NULL };
    KRPC::Request rq(SpaceCenterService, "Vessel_get_Resources", args);
    sendRequest(rq);
    return getIntResponse();
  }
  float resources_Amount(uint32_t vessel_resource, char *resource_name) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_resource)), new KRPC::Argument(1, new KRPC::pbBytes(resource_name)), NULL };
    KRPC::Request rq(SpaceCenterService, "Resources_Amount", args);
    sendRequest(rq);
    return getFloatResponse();
  }
  // control
  uint32_t control(uint32_t active_vessel) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(active_vessel)), NULL };
    KRPC::Request rq(SpaceCenterService, "Vessel_get_Control", args);
    sendRequest(rq);
    return getIntResponse();
  }
  void activateNextStage(uint32_t vessel_control) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_control)), NULL };
    KRPC::Request rq(SpaceCenterService, "Control_ActivateNextStage", args);
    sendRequest(rq);
    getIntResponse();
  }
  float getThrottle(uint32_t vessel_control) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_control)), NULL };
    KRPC::Request rq(SpaceCenterService, "Control_get_Throttle", args);
    sendRequest(rq);
    return getFloatResponse();
  }
  void setThrottle(uint32_t vessel_control, float throttle) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(vessel_control)), new KRPC::Argument(1, new KRPC::pbBytes(throttle)), NULL };
    KRPC::Request rq(SpaceCenterService, "Control_set_Throttle", args);
    sendRequest(rq);
    getIntResponse();
  }
  // autopilot
  uint32_t autoPilot(uint32_t active_vessel) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(active_vessel)), NULL };
    KRPC::Request rq(SpaceCenterService, "Vessel_get_AutoPilot", args);
    sendRequest(rq);
    return getIntResponse();
  }
  void autoPilot_Disengage(uint32_t autopilot) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(autopilot)), NULL };
    KRPC::Request rq(SpaceCenterService, "AutoPilot_Disengage", args);
    sendRequest(rq);
    getIntResponse();
  }
  void autoPilot_Engage(uint32_t autopilot) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(autopilot)), NULL };
    KRPC::Request rq(SpaceCenterService, "AutoPilot_Engage", args);
    sendRequest(rq);
    getIntResponse();
  }
  void autoPilot_TargetPitchAndHeading(uint32_t autopilot, float pitch, float heading) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(autopilot)), new KRPC::Argument(1, new KRPC::pbBytes(pitch)), new KRPC::Argument(2, new KRPC::pbBytes(heading)), NULL };
    KRPC::Request rq(SpaceCenterService, "AutoPilot_TargetPitchAndHeading", args);
    sendRequest(rq);
    getIntResponse();
  }
  void autoPilot_set_TargetPitch(uint32_t autopilot, float pitch) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(autopilot)), new KRPC::Argument(1, new KRPC::pbBytes(pitch)), NULL };
    KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetPitch", args);
    sendRequest(rq);
    getIntResponse();
  }
  void autoPilot_set_TargetHeading(uint32_t autopilot, float heading) {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytes(autopilot)), new KRPC::Argument(1, new KRPC::pbBytes(heading)), NULL };
    KRPC::Request rq(SpaceCenterService, "AutoPilot_set_TargetHeading", args);
    sendRequest(rq);
    getIntResponse();
  }
}
