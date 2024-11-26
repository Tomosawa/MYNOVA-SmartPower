<template>
  <v-app>
    <v-main>
      <v-app-bar>
        <template v-slot:prepend>
          <v-app-bar-nav-icon icon="menu"></v-app-bar-nav-icon>
        </template>

        <v-app-bar-title>{{title}}</v-app-bar-title>

        <template v-slot:append>
        <!--  <v-btn icon>
            <img src="@/assets/dots.svg" />
          </v-btn> -->
        </template>
      </v-app-bar>

      <router-view></router-view>

      <v-bottom-navigation v-model="naviIndex" color="teal" grow fixed app>

        <v-btn id="btn-powerinfo" @click="naviBarClick($event)">
          <!-- <img src="@/assets/powerinfo.svg" width="24" height="24" class="svg-icon"/> -->
          <inline-svg :src="powerinfo" width="24" height="24"></inline-svg>
          <span>电源信息</span>
        </v-btn>

        <v-btn id="btn-powersetting" @click="naviBarClick($event)">
          <!-- <img src="@/assets/powersetting.svg" width="24" height="24" class="svg-icon"/> -->
          <inline-svg :src="powersetting" width="24" height="24"></inline-svg>
          <span>电源设置</span>
        </v-btn>

        <v-btn id="btn-wificonfig" @click="naviBarClick($event)">
          <!-- <img src="@/assets/wifisetting.svg" width="24" height="24" class="svg-icon"/> -->
          <inline-svg :src="wifisetting" width="24" height="24"></inline-svg>
          <span>WIFI设置</span>
        </v-btn>

      </v-bottom-navigation>
    </v-main>
  </v-app>
</template>

<script>
  import {ref,onMounted,onUpdated,onUnmounted} from 'vue';
  import {useTheme} from 'vuetify';
  import {useRoute,useRouter} from 'vue-router';
  import InlineSvg from 'vue-inline-svg';
  import icon_powerinfo from './assets/powerinfo.svg';
  import icon_powersetting from './assets/powersetting.svg';
  import icon_wifisetting from './assets/wifisetting.svg';
  export default {
    components: {
       InlineSvg,
    },
    data: () => ({
      naviIndex: 0,
      title: "电源信息",
      powerinfo: icon_powerinfo,
      powersetting: icon_powersetting,
      wifisetting: icon_wifisetting,
    }),
    methods: {
      svgLoaded(){
        console.log('svgLoaded');
        console.log(darkModeEnable);
        const svgElements = document.querySelectorAll('path');
        console.log('svgnum:' + svgElements.length);
        svgElements.forEach(svg => {
          console.log('SVG');
          svg.style.setProperty('fill', darkModeEnable ? '#FFF' : '#000');
          svg.style.setProperty('stroke', darkModeEnable? '#fff' : '#000');
        });
      },
      naviBarClick(event) {
        var btnId = event.currentTarget.getAttribute('Id');
        // 执行其他逻辑
        switch (btnId) {
          case "btn-powerinfo":
            this.title = "电源信息";
            this.$router.push('/');
            break;
          case "btn-powersetting":
            this.title = "电源设置";
            this.$router.push('/powersetting');
            break;
          case "btn-wificonfig":
            this.title = "WIFI设置";
            this.$router.push('/wificonfig');
            break;
        }
      },
    },
    setup() {
      const theme = useTheme();
      const route = useRoute();
      const router = useRouter();
      const darkTheme = ref(false);
      const title = ref('');
      const naviIndex = ref(0);
      const setTheme = () => {
        const darkMediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
        darkTheme.value = darkMediaQuery.matches;
        theme.global.name.value = darkTheme.value ? 'dark' : 'light';
      };
      onMounted(() => {
        setTheme();
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', setTheme);
        router.beforeEach((to, from, next) => {
          switch (to.path) {
            case "/":
              title.value = "电源信息";
              naviIndex.value = 0;
              break;
            case "/powersetting":
              title.value = "电源设置";
              naviIndex.value = 1;
              break;
            case "/wificonfig":
              title.value = "WIFI设置";
              naviIndex.value = 2;
              break;
          }
          next();
        });
        
        // 阻止双指缩放
        document.addEventListener('gesturestart', function(event) {
          event.preventDefault();
        });

        // 阻止双击放大
        let lastTouchEnd = 0;
        document.addEventListener('touchend', function(event) {
          const now = Date.now();
          if (now - lastTouchEnd <= 300) {
            event.preventDefault();
          }
          lastTouchEnd = now;
        }, { passive: false });

        // 阻止双指缩放
        document.addEventListener('touchstart', function(event) {
          if (event.touches.length > 1) {
            event.preventDefault();
          }
        }, { passive: false });

        // 阻止双指缩放和移动时的缩放
        document.addEventListener('touchmove', function(event) {
          if (event.touches.length > 1) {
            event.preventDefault();
          }
        }, { passive: false });

        // 阻止双指缩放的另一种情况
        document.addEventListener('gesturechange', function(event) {
          event.preventDefault();
        });

        // 阻止双指缩放结束
        document.addEventListener('gestureend', function(event) {
          event.preventDefault();
        });
      });
      onUpdated(() => {
        console.log("onUpdated");
      });
      onUnmounted(() => {
        window.matchMedia('(prefers-color-scheme: dark)').removeEventListener('change', setTheme);
      });
      // 在 setup 函数中返回你在模板中需要使用的数据和函数
      return {
        title,
        naviIndex,
      };
    },
  }
</script>

<style>
  /* 亮色模式 */
/*  svg path {
    fill: black;
  } */
  /* 暗黑模式 */
  @media (prefers-color-scheme: dark) {
    svg path {
      fill: white;
    }
  }
  @media (prefers-color-scheme: light){
    svg path {
      fill: black;
    }
  }
</style>
