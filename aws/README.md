# AWS IOT Setup
This page provides information about how to setup, and later delete, an AWS IOT device and all required resources to run the iot-display solution on an ESP32.

# Disclaimer
If you execute commands described here or run scripts included in this repository, you do so at your own risk. I'm not reposible to keep your credit card free from any charges!

# Create Resources
To run this iot-display solution we need following AWS IOT resources.
- A Thing, which represents our device (ESP32)
- A Policy, to define permissions of our device
- A Certificate, to be used for connections to AWS IOT
Together with a certificate a private/pubic key pair is generated and have to be used for authentication when a device try to connect to AWS IOT.

## Create a Thing at AWS IOT
At first create a new thing on AWS IOT which represents our device.
```bash
aws iot create-thing \
    --thing-name <ThingName>
```
## Create a certificate
A certificate is used to authenticate a device while connecting to AWS IOT. In addition to the certificate itself you'll receive a private/public key pair.
Content from certificate file and private key have to be added to definitions in aws_iot_certs.h
```bash
aws iot create-keys-and-certificate \
    --set-as-active \
    --certificate-pem-outfile "<ThingName>.cert.pem" \
    --public-key-outfile "<ThingName>.public.key" \
    --private-key-outfile "<ThingName>.private.key"
```

## Create a policy
A üolicy is required to define which resources a device is allowed to use, e.g. which topics a device can subsreibe to.
File device_policy.json contains a policy template which wil be used in create.cli.sh to generate a device policy.
```bash
aws iot create-policy \
    --policy-name <PolicyName> \
    --policy-document <JSON>
```

## Attach your thing to a certificate
To put all pieces together attach our thing to the certificate.
```bash
aws iot attach-thing-principal \
    --thing-name <ThingName> \
    --principal <CertificateArn>
```

## Attach a policy to a certificate
Our policy have to be attached to the certificate as well.
```bash
aws iot attach-policy \
    --policy-name <PolicyName> \
    --target <CertificateArn>
```

## Create device shadow with default settings
To be able to use an AWS IOT device shadow to update settings, e.g. deep sleep time, you've to create this shadow first.
Here "settings" is used as shadow name. File device_shadow.json contains an example shadow payload.
```bash
aws iot-data update-thing-shadow \
    --thing-name <ThingName> \
    --shadow-name settings \
    --cli-binary-format raw-in-base64-out \
    --payload '{"state":{"desired":{"deep_sleep_seconds":60}}}' \
    <ThingName>_settings.json
```

# Fetch additional resources
In addition to a thing, certificate, keys and a policy we've to fetch AWS IOT endpoint for used region and the AWS Root CA certificate.

## Get AWS IOT endpoint for a specific region
Following command returns AWS IOT endpoint which have to be added to the aws_iot_client.h in iot-display sources.
If AWS_REGION is not set, you've to add --region param to specify your AWS region.
```bash
aws iot describe-endpoint \ 
    --endpoint-type iot:Data-ATS
```

## Get AWS Root CA certificate
Following command downloads AWS Root CA certificate to AmazonRootCA1.pem. This certificate have to be added to definitions in aws_iot_certs.h.
```bash
curl -o AmazonRootCA1.pem https://www.amazontrust.com/repository/AmazonRootCA1.pem
```

# Delete Resources
Run following commands to clean up all previous created AWS reqources. You need to know used names, ids and arns.

## Detach thing from certificate
Before you can start to delete a thing, policy or certificate you've to detach then. Following command detaches a thing from a certificate.
```bash
aws iot detach-thing-principal \
    --thing-name <ThingName> \
    --principal <CertificateArn>
```

## Detach policy from certificate
Our policy have to be detached from certificate as well.
```bash
aws iot detach-policy \
    --policy-name <PolicaName> \
    --target <CertificateArn>
```

# Deactive certificate
Deactivate our certificate before delete it.
```bash
aws iot update-certificate \
    --certificate-id <CertificateId> \
    --new-status "INACTIVE"
```

# Delete certificate by id
Now, delete the certificate.
```bash
aws iot delete-certificate \
    --certificate-id <CertificateId> \
    --force-delete
```

# Delete thing by name
Delete our thing by its name. 
```bash
aws iot delete-thing \
    --thing-name <ThingName>
```

# Delete policy by name
And delete device policy by its name.
```bash
aws iot delete-policy \
    --policy-name <PolicyName>
``` 

# Scripts 
All commands described above are included in a setup (create.cli.sh) and a clean up script (delete.cli.sh).

## create.cli.sh
This script creates a new thing, a policy and all required certs and keys. You have to adjust value for THING_NAME variable before you run this script.
Client certificate and key files are generated in local directory and have to be added to aws_iot_certs.h before you compile and upload your sketch to an EPS32.

## delete.cli.sh
To clean up all resources, thing, certtficate and policy, you can use delete.cli.sh.
If there's a properties file, generated by create.cli.sh, this file is used to get all required names, ids and arns for clean up. Without such a file you've to specify all this vaules in the scripts.

# Version Info
All this commands has been created and tested for AWS CLI version 2.7.24 using AWS CLoudShell.