import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core

# Define the namespace based on C++ namespace
tallsequoia_ns = cg.esphome_ns.namespace('tallsequoia')

# Define the class within the created namespace
FTMSExerciseBike = tallsequoia_ns.class_('FTMSExerciseBike', cg.PollingComponent, 
    filename="ftms_exercise_bike.h")
CONFIG_SCHEMA = cv.Schema({})

def to_code(config):
    var = cg.new_Pvariable(FTMSExerciseBike)
    cg.add(var.set_update_interval(1000))  # Update interval in milliseconds
