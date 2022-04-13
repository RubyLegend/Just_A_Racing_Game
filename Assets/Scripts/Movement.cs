using System.Collections;

using System.Collections.Generic;

using UnityEngine;
public class Movement : MonoBehaviour

{

   

public GameObject FrontLeftWheel;

public GameObject FrontRightWheel;
public GameObject RearLeftWheel;

public GameObject RearRightWheel;
     WheelCollider WheelFL;
     WheelCollider WheelFR;
     WheelCollider WheelRL;

     WheelCollider WheelRR;
     Transform meshFL;

     Transform meshFR;

     Transform meshRL;

     Transform meshRR;


    public bool motor; // is this wheel attached to motor?

    public bool steering; // does this wheel apply steer angle?
    public float maxMotorTorque;

   public float maxSteeringAngle;
    public Transform com;
   public bool isBrake;

    public bool handBrake;
    public bool mainBrake;

         
    public float maxSpeed;

    float speed;

   

    void Start()

    {

        gameObject.GetComponent<Rigidbody>().centerOfMass = com.localPosition;

       

WheelFL = FrontLeftWheel.GetComponent<WheelCollider>();
WheelFR = FrontRightWheel.GetComponent<WheelCollider>();
WheelRL = RearLeftWheel.GetComponent<WheelCollider>();
WheelRR = RearRightWheel.GetComponent<WheelCollider>();



meshFL = FrontLeftWheel.transform;

meshFR = FrontRightWheel.transform;

meshRL = RearLeftWheel.transform;

meshRR = RearRightWheel.transform;
    }
    // Update is called once per frame
    private void FixedUpdate()

    {

    

        float motor = maxMotorTorque * Input.GetAxis("Vertical");

        float steering = maxSteeringAngle * Input.GetAxis("Horizontal");
        ApplyRotation(WheelFL, meshFL);

        ApplyRotation(WheelFR, meshFR);

        ApplyRotation(WheelRL, meshRL);

        ApplyRotation(WheelRR, meshRR);




        if (motor == 0)

        {

            WheelFL.brakeTorque = 8000;

            WheelFR.brakeTorque = 8000;
            WheelRL.brakeTorque = 8000;

            WheelRR.brakeTorque = 8000;


        }

        else

        {

            WheelFL.brakeTorque = 0;

            WheelFR.brakeTorque = 0;

            WheelRL.brakeTorque = 0;

            WheelRR.brakeTorque = 0;
        }
        WheelFL.steerAngle = steering;

        WheelFR.steerAngle = steering;


        WheelRL.motorTorque = motor;

            WheelRR.motorTorque = motor;
        if (Input.GetButton("Jump") || isBrake == true)

        {

            WheelRL.brakeTorque = 25000;

            WheelRR.brakeTorque = 25000;

        }
        if (handBrake == true)

        {

            WheelRL.brakeTorque = 250000;

            WheelRR.brakeTorque = 250000;

            GetComponent<Rigidbody>().isKinematic = true;

        }


        if(mainBrake == true)

        {

            WheelRL.brakeTorque = 250000;

            WheelRR.brakeTorque = 250000;

        }

        else

        {

            WheelRL.brakeTorque = 0;

            WheelRR.brakeTorque = 0;

        }
    }


    public void BrakeHold()

    {

        isBrake = true;

    }
   public void ReleaseBrake()

    {

        isBrake = false;

    }
    public void MainBrakeClick()

    {

        mainBrake = true;

    }
    public void MainBrakeReleased()

    {

        mainBrake = false;

    }


    public void ApplyRotation(WheelCollider wheel,Transform visualWheel)

    {

        Vector3 position;

        Quaternion rotation;

        wheel.GetWorldPose(out position, out rotation);

        visualWheel.transform.position = position;

        visualWheel.transform.rotation = rotation;



    }
}