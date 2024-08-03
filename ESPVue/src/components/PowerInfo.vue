<template>
  <v-card class="mx-auto" >

    <v-list lines="two">
      <v-list-subheader inset>基础信息</v-list-subheader>

      <v-list-item title="制造商ID" subtitle="MFR_ID">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/maf.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_ID}}</div>
        </template>
      </v-list-item>

      <v-list-item title="型号" subtitle="MFR_MODEL">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/model.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_MODEL}}</div>
        </template>
      </v-list-item>

      <v-list-item title="修订号" subtitle="MFR_REVISION">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/code.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_REVISION}}</div>
        </template>
      </v-list-item>

      <v-list-item title="产地" subtitle="MFR_LOCATION">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/location.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_LOCATION}}</div>
        </template>
      </v-list-item>

      <v-list-item title="生产日期" subtitle="MFR_DATE">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/date.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_DATE}}</div>
        </template>
      </v-list-item>

      <v-list-item title="序列号" subtitle="MFR_SERIAL">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/serial.svg" />
          </v-avatar>
        </template>

        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.MFR_SERIAL}}</div>
        </template>
      </v-list-item>

      <v-divider inset></v-divider>


      <v-list-subheader inset>输入输出</v-list-subheader>

      <v-list-item title="输入电压" subtitle="READ_VIN">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/vin.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_VIN}} V</div>
        </template>
      </v-list-item>

      <v-list-item title="输入电流" subtitle="READ_IIN">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/iin.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_IIN}} A</div>
        </template>
      </v-list-item>

      <v-list-item title="输入功率" subtitle="READ_PIN">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/win.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_PIN}} W</div>
        </template>
      </v-list-item>

      <v-list-item title="输出电压" subtitle="READ_VOUT">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/vout.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_VOUT}} V</div>
        </template>
      </v-list-item>

      <v-list-item title="输出电流" subtitle="READ_IOUT">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/iout.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_IOUT}} A</div>
        </template>
      </v-list-item>

      <v-list-item title="输出功率" subtitle="READ_POUT">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/wout.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_POUT}} W</div>
        </template>
      </v-list-item>

      <v-list-subheader inset>温度相关</v-list-subheader>

      <v-list-item title="温度1" subtitle="READ_TEMPERATURE_1">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/temp.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_TEMPERATURE_1}} ℃</div>
        </template>
      </v-list-item>

      <v-list-item title="温度2" subtitle="READ_TEMPERATURE_2">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/temp.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_TEMPERATURE_2}} ℃</div>
        </template>
      </v-list-item>

      <v-list-item title="温度3" subtitle="READ_TEMPERATURE_3">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/temp.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_TEMPERATURE_3}} ℃</div>
        </template>
      </v-list-item>

      <v-list-item title="风扇1" subtitle="READ_FAN_SPEED_1">
        <template v-slot:prepend>
          <v-avatar color="grey-lighten-1">
            <img src="@/assets/fan.svg" />
          </v-avatar>
        </template>
        <template v-slot:append>
          <div class="text-subtitle-3">{{psuInfo.READ_FAN_SPEED_1}} RPM</div>
        </template>
      </v-list-item>

    </v-list>
  </v-card>
</template>

<script>
  import axios from 'axios';
  export default {
    data: () => ({
      psuInfo: {
        MFR_ID: '',
        MFR_MODEL: '',
        MFR_REVISION: '',
        MFR_LOCATION: '',
        MFR_DATE: '0000',
        MFR_SERIAL: '000000',
        READ_VIN: '000.0',
        READ_IIN: '0.000',
        READ_PIN: '0.000',
        READ_VOUT: '0.000',
        READ_IOUT: '0.000',
        READ_POUT: '0.000',
        READ_TEMPERATURE_1: '0.0',
        READ_TEMPERATURE_2: '0.0',
        READ_TEMPERATURE_3: '0.0',
        READ_FAN_SPEED_1: '0',
      },
      intervalId : 0,
    }),
    mounted() {
      this.fetchData();
      this.intervalId = setInterval(this.fetchData, 3000); // 每 3 秒发送一次请求
    },
    methods: {
      async fetchData() {
        try {
          const response = await axios.get('/api/psuinfo');
          this.psuInfo = response.data;
          console.log(this.psuInfo);
        } catch (error) {
          console.error('There was an error fetching the data:', error);
        }
      }
    },
    beforeUnmount() {
      clearInterval(this.intervalId); // 组件卸载前清除定时器
    }
  }
</script>
