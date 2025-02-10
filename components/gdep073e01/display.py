# components/gdep073e01/display.py
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import display, spi
from esphome import pins, core
from esphome.const import (
    CONF_BUSY_PIN,
    CONF_DC_PIN,
    CONF_FULL_UPDATE_EVERY,
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_RESET_DURATION,
    CONF_RESET_PIN,
)
CODEOWNERS = ["@your_username"]
DEPENDENCIES = ["spi", "gpio"]  # Explicitly declare both

gdep073e01_ns = cg.esphome_ns.namespace("gdep073e01")
GDEP073E01 = gdep073e01_ns.class_("GDEP073E01", display.DisplayBuffer, spi.SPIDevice)

CONF_BUSY_PIN = "busy_pin"
CONF_RESET_PIN = "reset_pin"
CONF_DC_PIN = "dc_pin"


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(GDEP073E01),
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_BUSY_PIN): pins.gpio_input_pin_schema,
            cv.Optional(CONF_FULL_UPDATE_EVERY): cv.int_range(min=1, max=4294967295),
            cv.Optional(CONF_RESET_DURATION): cv.All(
                cv.positive_time_period_milliseconds,
                cv.Range(max=core.TimePeriod(milliseconds=500)),
            ),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema()),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)

FINAL_VALIDATE_SCHEMA = spi.final_validate_device_schema(
    "gdep073e01", require_miso=False, require_mosi=True
)


async def to_code(config):

    await display.register_display(var, config)
    await spi.register_spi_device(var, config)

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))
    if CONF_BUSY_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_BUSY_PIN])
        cg.add(var.set_busy_pin(reset))
    if CONF_FULL_UPDATE_EVERY in config:
        cg.add(var.set_full_update_every(config[CONF_FULL_UPDATE_EVERY]))
    if CONF_RESET_DURATION in config:
        cg.add(var.set_reset_duration(config[CONF_RESET_DURATION]))