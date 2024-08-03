import { createRouter, createWebHistory } from 'vue-router'
import PowerInfo from '@/components/PowerInfo.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'PowerInfo',
      component: PowerInfo
    },
    {
      path: '/powersetting',
      name: 'PowerSetting',
      component: () => import('../components/PowerSetting.vue')
    },
    {
      path: '/wificonfig',
      name: 'WIFIConfig',
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../components/WIFIConfig.vue')
    }
  ]
})

export default router
