import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core

ftms_exercise_bike_ns = cg.esphome_ns.namespace('ftms_exercise_bike')
FTMSExerciseBike = ftms_exercise_bike_ns.class_('FTMSExerciseBike', cg.PollingComponent, 
    filename="ftms_exercise_bike.h")

CONFIG_SCHEMA = cv.Schema({})

def to_code(config):
    var = cg.new_Pvariable(FTMSExerciseBike)
    cg.add(var.set_update_interval(1000))  # Update interval in milliseconds
