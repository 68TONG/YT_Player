pragma Singleton
import QtQml
import QtQuick

QtObject {

    // // M3 linear: cubic-bezier(0, 0, 1, 1)
    // readonly property var easingLinear: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.0, 0.0, 1.0, 1.0, 1.0, 1.0]
    //     })

    // // M3 standard easing: cubic-bezier(0.2, 0, 0, 1)
    // readonly property var easingStandard: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.2, 0.0, 0.0, 1.0, 1.0, 1.0]
    //     })

    // // M3 standard accelerate: cubic-bezier(0.3, 0, 1, 1)
    // readonly property var easingStandardAccelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.3, 0.0, 1.0, 1.0, 1.0, 1.0]
    //     })

    // // M3 standard decelerate: cubic-bezier(0, 0, 0, 1)
    // readonly property var easingStandardDecelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.0, 0.0, 0.0, 1.0, 1.0, 1.0]
    //     })

    // // M3 emphasized path:
    // // M 0,0 C 0.05,0 0.133333,0.06 0.166666,0.4
    // //       C 0.208333,0.82 0.25,1 1,1
    // readonly property var easingEmphasized: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.05, 0.0, 0.133333, 0.06, 0.166666, 0.4, 0.208333, 0.82, 0.25, 1.0, 1.0, 1.0]
    //     })

    // // M3 emphasized accelerate: cubic-bezier(0.3, 0, 0.8, 0.15)
    // readonly property var easingEmphasizedAccelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.3, 0.0, 0.8, 0.15, 1.0, 1.0]
    //     })

    // // M3 emphasized decelerate: cubic-bezier(0.05, 0.7, 0.1, 1)
    // readonly property var easingEmphasizedDecelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.05, 0.7, 0.1, 1.0, 1.0, 1.0]
    //     })

    // // Legacy Material standard easing: cubic-bezier(0.4, 0, 0.2, 1)
    // readonly property var easingLegacy: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.4, 0.0, 0.2, 1.0, 1.0, 1.0]
    //     })

    // // Legacy accelerate: cubic-bezier(0.4, 0, 1, 1)
    // readonly property var easingLegacyAccelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.4, 0.0, 1.0, 1.0, 1.0, 1.0]
    //     })

    // // Legacy decelerate: cubic-bezier(0, 0, 0.2, 1)
    // readonly property var easingLegacyDecelerate: ({
    //         type: Easing.BezierSpline,
    //         bezierCurve: [0.0, 0.0, 0.2, 1.0, 1.0, 1.0]
    //     })

    // Material 3 expressive spatial motion curves.
    // duration is token metadata; it can also be assigned to animation.duration.
    // Expressive fast spatial: cubic-bezier(0.42, 1.67, 0.21, 0.90), 350ms
    readonly property var springFastEx: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.42, 1.67, 0.21, 0.90, 1.0, 1.0]
        })

    // Expressive default spatial: cubic-bezier(0.38, 1.21, 0.22, 1.00), 500ms
    readonly property var springDefaultEx: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.38, 1.21, 0.22, 1.00, 1.0, 1.0]
        })

    // Expressive slow spatial: cubic-bezier(0.39, 1.29, 0.35, 0.98), 650ms
    readonly property var springSlowEx: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.39, 1.29, 0.35, 0.98, 1.0, 1.0]
        })

    // Standard fast spatial: cubic-bezier(0.27, 1.06, 0.18, 1.00), 350ms
    readonly property var springFast: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.27, 1.06, 0.18, 1.00, 1.0, 1.0]
        })

    // Standard default spatial: cubic-bezier(0.27, 1.06, 0.18, 1.00), 500ms
    readonly property var springDefault: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.27, 1.06, 0.18, 1.00, 1.0, 1.0]
        })

    // Standard slow spatial: cubic-bezier(0.27, 1.06, 0.18, 1.00), 750ms
    readonly property var springSlow: ({
            type: Easing.BezierSpline,
            bezierCurve: [0.27, 1.06, 0.18, 1.00, 1.0, 1.0]
        })

    // MD3 standard fast spatial.
    component SpringAnimation_Fast: SpringAnimation {
        spring: 5.000
        damping: 0.274
        mass: 0.497
    }

    // MD3 expressive fast spatial.
    component SpringAnimation_FastEx: SpringAnimation {
        spring: 5.000
        damping: 0.240
        mass: 0.633
    }

    // MD3 standard default spatial.
    component SpringAnimation_Default: SpringAnimation {
        spring: 5.000
        damping: 0.373
        mass: 0.822
    }

    // MD3 expressive default spatial.
    component SpringAnimation_DefaultEx: SpringAnimation {
        spring: 3.939
        damping: 0.347
        mass: 1.000
    }

    // MD3 standard slow spatial.
    component SpringAnimation_Slow: SpringAnimation {
        spring: 3.125
        damping: 0.345
        mass: 1.000
    }

    // MD3 expressive slow spatial.
    component SpringAnimation_SlowEx: SpringAnimation {
        spring: 2.307
        damping: 0.275
        mass: 1.000
    }

    // Approximate offline-fitted mapping from official MD3 spring token pairs
    // to Qt Quick SpringAnimation. Use a fresh instance or register as a singleton.
    readonly property var _MD3Spring_fitTable: [
        // MD3 motion scheme and spring tokens -> QML spring, damping, mass
        {
            md3Damping: 0.6,
            md3Stiffness: 800,
            spring: 5.000,
            damping: 0.240,
            mass: 0.633
        } // expressive fast spatial
        ,
        {
            md3Damping: 0.8,
            md3Stiffness: 380,
            spring: 3.939,
            damping: 0.347,
            mass: 1.000
        } // expressive default spatial
        ,
        {
            md3Damping: 0.8,
            md3Stiffness: 200,
            spring: 2.307,
            damping: 0.275,
            mass: 1.000
        } // expressive slow spatial
        ,
        {
            md3Damping: 1.0,
            md3Stiffness: 3800,
            spring: 5.000,
            damping: 0.199,
            mass: 0.278
        } // fast effects
        ,
        {
            md3Damping: 1.0,
            md3Stiffness: 1600,
            spring: 5.000,
            damping: 0.285,
            mass: 0.479
        } // default effects
        ,
        {
            md3Damping: 1.0,
            md3Stiffness: 800,
            spring: 5.000,
            damping: 0.387,
            mass: 0.779
        } // slow effects
        ,
        {
            md3Damping: 0.9,
            md3Stiffness: 1400,
            spring: 5.000,
            damping: 0.274,
            mass: 0.497
        } // standard fast spatial
        ,
        {
            md3Damping: 0.9,
            md3Stiffness: 700,
            spring: 5.000,
            damping: 0.373,
            mass: 0.822
        } // standard default spatial
        ,
        {
            md3Damping: 0.9,
            md3Stiffness: 300,
            spring: 3.125,
            damping: 0.345,
            mass: 1.000
        }  // standard slow spatial
    ]

    // Returns null for values that have not been fitted.
    // To add custom MD3 values, run md3_qml_spring_fit.py and add the result here.
    function getSpringValue(md3Damping: real, md3Stiffness: int): var {
        for (let i = 0; i < _MD3Spring_fitTable.length; ++i) {
            const item = _MD3Spring_fitTable[i];
            if (Math.abs(item.md3Damping - md3Damping) < 0.0001 && item.md3Stiffness === md3Stiffness) {
                return {
                    spring: item.spring,
                    damping: item.damping,
                    mass: item.mass
                };
            }
        }
        return null;
    }
}
